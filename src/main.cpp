#include <UrhoBits/UrhoAppFramework/UrhoAppFramework.hpp>

#include <UrhoBits/TreeGenerator/TreeGenerator.hpp>
#include "TreeEditState.hpp"

#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "AppSettings.hpp"


class TreeGen : public UrhoBits::UrhoAppFramework {
    URHO3D_OBJECT(TreeGen, UrhoAppFramework);
public:
    TreeGen (Urho3D::Context* pContext)
        : UrhoBits::UrhoAppFramework (pContext),
          _cfg_detail ("TreeGen.ini"),
          _cfg(_cfg_detail),
          _treeEditState(pContext, &_stateMgr, &_inputMgr, &_cfg) {
    }

    void start () override {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= (ImGuiConfigFlags_DockingEnable);


        setupScene();
        _treeEditState.setCameraNode(_cameraNode);

       _stateMgr.pushState(&_treeEditState);
    }

    void setupScene() {
        _scene = new Urho3D::Scene(context_);

        _scene->CreateComponent<Urho3D::Octree>();

        Urho3D::Node* lightNode = _scene->CreateChild("DirectionalLight");
        lightNode->SetDirection(Urho3D::Vector3(1.f, -0.3f, 0.f));

        Urho3D::Light* light = lightNode->CreateComponent<Urho3D::Light>();
        light->SetLightType(Urho3D::LightType::LIGHT_DIRECTIONAL);


        _cameraNode = _scene->CreateChild("Camera");
        auto camera = _cameraNode->CreateComponent<Urho3D::Camera>();
        GetSubsystem<Urho3D::Renderer>()
            ->SetViewport(0, new Urho3D::Viewport(context_,
                                                  _scene,
                                                  camera));

        _cameraNode->SetPosition(Urho3D::Vector3(10, 10, 10));
        _cameraNode->LookAt(Urho3D::Vector3(0, 0, 0));
        // remove fog
        auto zone = _scene->GetOrCreateComponent<Urho3D::Zone>();
        const auto maxfloat = std::numeric_limits<float>::max();
        const auto minfloat = std::numeric_limits<float>::min();
        zone->SetBoundingBox(Urho3D::BoundingBox(-maxfloat, maxfloat));
        zone->SetFogStart(maxfloat);
        zone->SetFogEnd(maxfloat);

        zone->SetAmbientColor(Urho3D::Color(0.4f, 0.4f, 0.4f));

        Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
        cache->SetAutoReloadResources(true);
    }

private:
    Urho3D::Scene *_scene = nullptr;

    Urho3D::Node *_cameraNode = nullptr;
    //MainMenu _mainMenu { context_, &_stateMgr, &_inputMgr };
    TreeEditState _treeEditState;

    VcppBits::Settings _cfg_detail;
    AppSettings _cfg;
};

URHO3D_DEFINE_APPLICATION_MAIN(TreeGen);
