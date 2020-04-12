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
        _cfg.ambient.addUpdateHandler(
            this, std::bind(&TreeGen::onAmbientSettingUpdate, this));
        _cfg.fog_color.addUpdateHandler(
            this, std::bind(&TreeGen::onFogColorSettingUpdate, this));
        _cfg.shadow_intensity.addUpdateHandler(
            this, std::bind(&TreeGen::onShadowIntensitySettingUpdate, this));
        _cfg.light_direction.addUpdateHandler(
            this, std::bind(&TreeGen::onLightDirectionSettingUpdate, this));
    }


    void onAmbientSettingUpdate () {
        _zone->SetAmbientColor(Urho3D::Color(_cfg.ambient_vector3));
    }

    void onLightDirectionSettingUpdate () {
        _light->GetNode()->SetDirection(_cfg.light_direction_vector3);
    }

    void onFogColorSettingUpdate () {
        _zone->SetFogColor(Urho3D::Color(_cfg.fog_color_vector3));
    }

    void onShadowIntensitySettingUpdate () {
        _light->SetShadowIntensity(_cfg.shadow_intensity_float);
    }

    void onAutoExposureSettingUpdate () {
        _renderPath->SetEnabled("AutoExposure", _cfg.auto_exposure.getValue<V2::BoolValue>());
    }

    void onFxaaSettingUpdate () {
        _renderPath->SetEnabled("FXAA3", _cfg.fxaa.getValue<V2::BoolValue>());
    }

    void onSsaoSettingUpdate () {
        const float ssao = _cfg.ssao.getValue<V2::FloatValue>();
        if (ssao > 0.f) {
            _renderPath->SetEnabled("Ssao", true);
            _renderPath->SetShaderParameter("SsaoStrength", ssao);
        } else {
            _renderPath->SetEnabled("Ssao", false);
        }
    }

    void onVibranceSettingUpdate () {
        const float vibr = _cfg.vibrance.getValue<V2::FloatValue>();
        if (vibr > 0.f) {
            _renderPath->SetEnabled("Vibrance", true);
            _renderPath->SetShaderParameter("VibranceStrength", vibr);
        } else {
            _renderPath->SetEnabled("Vibrance", false);
        }
    }

    void initPostProcessingSettings () {
        onAutoExposureSettingUpdate();
        onSsaoSettingUpdate();
        onFxaaSettingUpdate();
        onVibranceSettingUpdate();
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
        lightNode->SetDirection(_cfg.light_direction_vector3);

        _light = lightNode->CreateComponent<Urho3D::Light>();
        _light->SetLightType(Urho3D::LightType::LIGHT_DIRECTIONAL);
        _light->SetCastShadows(true);
        _light->SetShadowIntensity(_cfg.shadow_intensity_float);


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
        _zone->SetFogColor(Urho3D::Color(_cfg.fog_color_vector3));
        _zone->SetFogStart(20.f);
        _zone->SetFogEnd(1000.f);

        // to remove fog:
        // const auto maxfloat = std::numeric_limits<float>::max();
        // zone->SetBoundingBox(Urho3D::BoundingBox(-maxfloat, maxfloat));
        // zone->SetFogStart(maxfloat);
        // zone->SetFogEnd(maxfloat);

        _zone->SetAmbientColor(Urho3D::Color(_cfg.ambient_vector3));

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
