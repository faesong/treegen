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
        using std::placeholders::_1;
        _cfg.auto_exposure.addUpdateHandler<V2::BoolValue>(
            this, std::bind(&TreeGen::onAutoExposureSettingUpdate, this, _1));
        _cfg.ssao.addUpdateHandler<V2::FloatValue>(
            this, std::bind(&TreeGen::onSsaoSettingUpdate, this, _1));
        _cfg.vibrance.addUpdateHandler<V2::FloatValue>(
            this, std::bind(&TreeGen::onVibranceSettingUpdate, this, _1));
        _cfg.fxaa.addUpdateHandler<V2::BoolValue>(
            this, std::bind(&TreeGen::onFxaaSettingUpdate, this, _1));
        _cfg.ambient.addUpdateHandler<Vector3Value>(
            this, std::bind(&TreeGen::onAmbientSettingUpdate, this, _1));
        _cfg.fog_color.addUpdateHandler<Vector3Value>(
            this, std::bind(&TreeGen::onFogColorSettingUpdate, this, _1));
        _cfg.shadow_intensity.addUpdateHandler<V2::FloatValue>(
            this, std::bind(&TreeGen::onShadowIntensitySettingUpdate, this, _1));
        _cfg.light_direction.addUpdateHandler<Vector3Value>(
            this, std::bind(&TreeGen::onLightDirectionSettingUpdate, this, _1));
    }


    void onAmbientSettingUpdate (const Urho3D::Vector3 &pAmbient) {
        _zone->SetAmbientColor(Urho3D::Color(pAmbient));
    }

    void onLightDirectionSettingUpdate (const Urho3D::Vector3 &pLightDir) {
        _light->GetNode()->SetDirection(pLightDir);
    }

    void onFogColorSettingUpdate (const Urho3D::Vector3 &pFogColor) {
        _zone->SetFogColor(Urho3D::Color(pFogColor));
    }

    void onShadowIntensitySettingUpdate (const float &pIntensity) {
        _light->SetShadowIntensity(pIntensity);
    }

    void onAutoExposureSettingUpdate (const bool &pAutoExposure) {
        _renderPath->SetEnabled("AutoExposure", pAutoExposure);
    }

    void onFxaaSettingUpdate (const bool &pFxaa) {
        _renderPath->SetEnabled("FXAA3", pFxaa);
    }

    void onSsaoSettingUpdate (const float pSsao) {
        if (pSsao > 0.f) {
            _renderPath->SetEnabled("Ssao", true);
            _renderPath->SetShaderParameter("SsaoStrength", pSsao);
        } else {
            _renderPath->SetEnabled("Ssao", false);
        }
    }

    void onVibranceSettingUpdate (const float pVibrance) {
        if (pVibrance > 0.f) {
            _renderPath->SetEnabled("Vibrance", true);
            _renderPath->SetShaderParameter("VibranceStrength", pVibrance);
        } else {
            _renderPath->SetEnabled("Vibrance", false);
        }
    }

    void initPostProcessingSettings () {
        _cfg_detail.triggerListeners<V2::BoolValue>("auto_exposure");
        _cfg_detail.triggerListeners<V2::FloatValue>("ssao");
        _cfg_detail.triggerListeners<V2::BoolValue>("fxaa");
        _cfg_detail.triggerListeners<V2::FloatValue>("vibrance");
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

        _light = lightNode->CreateComponent<Urho3D::Light>();
        _light->SetLightType(Urho3D::LightType::LIGHT_DIRECTIONAL);
        _light->SetCastShadows(true);

        _cfg_detail.triggerListeners<V2::FloatValue>("shadow_intensity");
        _cfg_detail.triggerListeners<Vector3Value>("light_direction");


        _cameraNode = _scene->CreateChild("Camera");
        auto camera = _cameraNode->CreateComponent<Urho3D::Camera>();
        GetSubsystem<Urho3D::Renderer>()
            ->SetViewport(0, new Urho3D::Viewport(context_,
                                                  _scene,
                                                  camera));

        _cameraNode->SetPosition(Urho3D::Vector3(10, 10, 10));
        _cameraNode->LookAt(Urho3D::Vector3(0, 0, 0));

        _zone = _scene->GetOrCreateComponent<Urho3D::Zone>();

        _zone->SetBoundingBox(Urho3D::BoundingBox(-100000, 100000));
        _cfg_detail.triggerListeners<Vector3Value>("fog_color");

        _zone->SetFogStart(20.f);
        _zone->SetFogEnd(1000.f);

        // to remove fog:
        // const auto maxfloat = std::numeric_limits<float>::max();
        // zone->SetBoundingBox(Urho3D::BoundingBox(-maxfloat, maxfloat));
        // zone->SetFogStart(maxfloat);
        // zone->SetFogEnd(maxfloat);

        _cfg_detail.triggerListeners<Vector3Value>("ambient_color");

        cache->SetAutoReloadResources(true);

        _renderPath = GetSubsystem<Urho3D::Renderer>()->GetViewport(0)->GetRenderPath();
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/SSAO.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/AutoExposure.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/FXAA3.xml"));
        _renderPath->Append(cache->GetResource<Urho3D::XMLFile>("PostProcess/Vibrance.xml"));

        initPostProcessingSettings();
    }

private:
    Urho3D::Scene *_scene = nullptr;

    Urho3D::Node *_cameraNode = nullptr;
    //MainMenu _mainMenu { context_, &_stateMgr, &_inputMgr };

    Settings2 _cfg_detail;
    AppSettings _cfg;

    RootState _rootState;
    TreeEditState _treeEditState;

    Urho3D::RenderPath *_renderPath;
    Urho3D::Zone *_zone;
    Urho3D::Light *_light;
};

URHO3D_DEFINE_APPLICATION_MAIN(TreeGen)
