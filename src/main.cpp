// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <UrhoBits/UrhoAppFramework/UrhoAppFramework.hpp>

#include <UrhoBits/TreeGenerator/TreeGenerator.hpp>
#include "TreeEditState.hpp"

#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "AppSettings.hpp"
#include "RootState.hpp"


class TreeGen : public UrhoBits::UrhoAppFramework {
    URHO3D_OBJECT(TreeGen, UrhoAppFramework);
public:
    TreeGen (Urho3D::Context* pContext)
        : UrhoBits::UrhoAppFramework (pContext),
          _cfg_detail ("TreeGen.ini"),
          _cfg(_cfg_detail),
          _rootState(&_stateMgr, &_cfg),
          _treeEditState(pContext, &_stateMgr, &_inputMgr, &_cfg) {
        _cfg.auto_exposure.addUpdateHandler(
            this, std::bind(&TreeGen::onAutoExposureSettingUpdate, this));
        _cfg.ssao.addUpdateHandler(
            this, std::bind(&TreeGen::onSsaoSettingUpdate, this));
        _cfg.vibrance.addUpdateHandler(
            this, std::bind(&TreeGen::onVibranceSettingUpdate, this));
        _cfg.fxaa.addUpdateHandler(
            this, std::bind(&TreeGen::onFxaaSettingUpdate, this));
    }

    void onAutoExposureSettingUpdate () {
        _renderPath->SetEnabled("AutoExposure", _cfg.auto_exposure.getBool());
    }

    void onFxaaSettingUpdate () {
        _renderPath->SetEnabled("FXAA3", _cfg.fxaa.getBool());
    }

    void onSsaoSettingUpdate () {
        const float ssao = _cfg.ssao.getFloat();
        if (ssao > 0.f) {
            _renderPath->SetEnabled("Ssao", true);
            _renderPath->SetShaderParameter("SsaoStrength", ssao);
        } else {
            _renderPath->SetEnabled("Ssao", false);
        }
    }

    void onVibranceSettingUpdate () {
        const float vibr = _cfg.vibrance.getFloat();
        if (vibr > 0.f) {
            _renderPath->SetEnabled("Vibrance", true);
            _renderPath->SetShaderParameter("VibranceStrength", vibr);
        } else {
            _renderPath->SetEnabled("Vibrance", false);
        }
    }

    void setup () override {
        engineParameters_[Urho3D::EP_MULTI_SAMPLE] = 8;
        engineParameters_[Urho3D::EP_FRAME_LIMITER] = true;
        engineParameters_[Urho3D::EP_REFRESH_RATE] = 200;
    }

    void start () override {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= (ImGuiConfigFlags_DockingEnable);


        setupScene();
        _treeEditState.setCameraNode(_cameraNode);

         _stateMgr.pushState(&_rootState);
         _stateMgr.update();
        _stateMgr.pushState(&_treeEditState);

        //SubscribeToEvent(Urho3D::E_UPDATE,
        //                         URHO3D_HANDLER(TreeGen,
        //                            update));
    }

    void setupScene() {
        Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

        _scene = new Urho3D::Scene(context_);

        _scene->CreateComponent<Urho3D::Octree>();

        Urho3D::Node* lightNode = _scene->CreateChild("DirectionalLight");
        lightNode->SetDirection(Urho3D::Vector3(1.f, -0.3f, 0.f));

        Urho3D::Light* light = lightNode->CreateComponent<Urho3D::Light>();
        light->SetLightType(Urho3D::LightType::LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowIntensity(0.0);


        _cameraNode = _scene->CreateChild("Camera");
        auto camera = _cameraNode->CreateComponent<Urho3D::Camera>();
        GetSubsystem<Urho3D::Renderer>()
            ->SetViewport(0, new Urho3D::Viewport(context_,
                                                  _scene,
                                                  camera));

        _cameraNode->SetPosition(Urho3D::Vector3(10, 10, 10));
        _cameraNode->LookAt(Urho3D::Vector3(0, 0, 0));

        auto zone = _scene->GetOrCreateComponent<Urho3D::Zone>();

        zone->SetBoundingBox(Urho3D::BoundingBox(-100000, 100000));
        zone->SetFogColor(Urho3D::Color(0.5f, 0.5f, 0.7f));
        zone->SetFogStart(20.f);
        zone->SetFogEnd(1000.f);

        // to remove fog:
        // const auto maxfloat = std::numeric_limits<float>::max();
        // zone->SetBoundingBox(Urho3D::BoundingBox(-maxfloat, maxfloat));
        // zone->SetFogStart(maxfloat);
        // zone->SetFogEnd(maxfloat);

        zone->SetAmbientColor(Urho3D::Color(0.45, 0.45, 0.5));

        cache->SetAutoReloadResources(true);

        _renderPath = GetSubsystem<Urho3D::Renderer>()->GetViewport(0)->GetRenderPath();
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/SSAO.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/AutoExposure.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/FXAA3.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/Vibrance.xml"));

        onAutoExposureSettingUpdate();
        onSsaoSettingUpdate();
        onVibranceSettingUpdate();

        Urho3D::Vector2 screenSize(GetSubsystem<Urho3D::Graphics>()->GetWidth(), GetSubsystem<Urho3D::Graphics>()->GetHeight());
        GetSubsystem<Urho3D::Renderer>()->GetViewport(0)->GetRenderPath()->SetShaderParameter("ScreenSize", screenSize);

        SubscribeToEvent(Urho3D::E_SCREENMODE,URHO3D_HANDLER(TreeGen,
                                                             screenSizeChanged));
    }

    void screenSizeChanged (Urho3D::StringHash eventType,
                            Urho3D::VariantMap& eventData) {
        int w=eventData[Urho3D::ScreenMode::P_WIDTH].GetInt();
        int h=eventData[Urho3D::ScreenMode::P_HEIGHT].GetInt();

        Urho3D::Vector2 screenSize(w, h);

        GetSubsystem<Urho3D::Renderer>()->GetViewport(0)->GetRenderPath()->SetShaderParameter("ScreenSize", screenSize);

    }

private:
    Urho3D::Scene *_scene = nullptr;

    Urho3D::Node *_cameraNode = nullptr;
    //MainMenu _mainMenu { context_, &_stateMgr, &_inputMgr };

    VcppBits::Settings _cfg_detail;
    AppSettings _cfg;

    RootState _rootState;
    TreeEditState _treeEditState;

    Urho3D::RenderPath *_renderPath;
};

URHO3D_DEFINE_APPLICATION_MAIN(TreeGen)
