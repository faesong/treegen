// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "TreeGen.hpp"


#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/SystemUI/SystemUI.h>


#include "TranslationIds.hpp"
#include <VcppBits/Translation/Translation.hpp>


TreeGen::TreeGen (Urho3D::Context* pContext)
    : UrhoBits::UrhoAppFramework (pContext),
      _tr (std::make_unique<VcppBits::Translation::Translation>()),
      _cfg_detail ("TreeGen.ini"),
      _cfg(_cfg_detail, _tr.get()),
      _rootState(_tr.get(), &_stateMgr, &_cfg),
      _treeEditState(pContext, &_stateMgr, &_inputMgr, &_cfg, _tr.get()) {
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
    _cfg.language.addUpdateHandler<V2::EnumStringValue>(
        this, std::bind(&TreeGen::onLanguageSettingUpdate, this, _1));
}

void TreeGen::onLanguageSettingUpdate (const std::string & /*pNewLang*/) {
    _reloadLanguageRequested = true;
}

void TreeGen::onAmbientSettingUpdate (const Urho3D::Vector3 &pAmbient) {
    _zone->SetAmbientColor(Urho3D::Color(pAmbient));
}

void TreeGen::onLightDirectionSettingUpdate (const Urho3D::Vector3 &pLightDir) {
    _light->GetNode()->SetDirection(pLightDir);
}

void TreeGen::onFogColorSettingUpdate (const Urho3D::Vector3 &pFogColor) {
    _zone->SetFogColor(Urho3D::Color(pFogColor));
}

void TreeGen::onShadowIntensitySettingUpdate (const float &pIntensity) {
    _light->SetShadowIntensity(1.f - pIntensity);
}

void TreeGen::onAutoExposureSettingUpdate (const bool &pAutoExposure) {
    _renderPath->SetEnabled("AutoExposure", pAutoExposure);
}

void TreeGen::onFxaaSettingUpdate (const bool &pFxaa) {
    _renderPath->SetEnabled("FXAA3", pFxaa);
}

void TreeGen::onSsaoSettingUpdate (const float pSsao) {
    if (pSsao > 0.f) {
        _renderPath->SetEnabled("Ssao", true);
        _renderPath->SetShaderParameter("SsaoStrength", pSsao);
    } else {
        _renderPath->SetEnabled("Ssao", false);
    }
}

void TreeGen::onVibranceSettingUpdate (const float pVibrance) {
    if (pVibrance > 0.f) {
        _renderPath->SetEnabled("Vibrance", true);
        _renderPath->SetShaderParameter("VibranceStrength", pVibrance);
    } else {
        _renderPath->SetEnabled("Vibrance", false);
    }
}

void TreeGen::initPostProcessingSettings () {
    _cfg_detail.triggerListeners<V2::BoolValue>("auto_exposure");
    _cfg_detail.triggerListeners<V2::FloatValue>("ssao");
    _cfg_detail.triggerListeners<V2::BoolValue>("fxaa");
    _cfg_detail.triggerListeners<V2::FloatValue>("vibrance");
}

void TreeGen::setup () {
    engineParameters_[Urho3D::EP_MULTI_SAMPLE] = 8;
    engineParameters_[Urho3D::EP_FRAME_LIMITER] = true;
    engineParameters_[Urho3D::EP_REFRESH_RATE] = 200;
    engineParameters_[Urho3D::EP_RESOURCE_PREFIX_PATHS] = ".;..";
}

void TreeGen::start () {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= (ImGuiConfigFlags_DockingEnable);

    auto fnt = GetSubsystem<Urho3D::SystemUI>()->AddFont("Data/Fonts/DroidSansMono.ttf",
                                                         io.Fonts->GetGlyphRangesDefault(),
                                                         18,
                                                         false);



    setupScene();
    _treeEditState.setCameraNode(_cameraNode);

    _stateMgr.pushState(&_rootState);
    _stateMgr.update();
    _stateMgr.pushState(&_treeEditState);
}

void TreeGen::setupScene() {
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


static void imrange_merge_string (ImWchar &pMax,
                                  ImWchar &pMin,
                                  const std::string &pWord) {
    for (auto& c : VcppBits::StringUtils::toUtf32(pWord)) {
        if (c > pMax) {
            pMax = c;
        }
        if (c < pMin) {
            pMin = c;
        }
    }
}

void TreeGen::endFrame () {
    // TODO20: refactor this PLEASE
    if (_reloadLanguageRequested) {
        // first make sure all language names can be rendered properly
        auto io = ImGui::GetIO();

        static std::vector<ImWchar> ranges;
        ranges.clear();

        for (auto& l : _tr->getLanguages()) {
            ImWchar max = 0;
            ImWchar min = std::numeric_limits<ImWchar>::max();
            imrange_merge_string(max, min, l);
            ranges.push_back(min);
            ranges.push_back(max);
        }



        const std::string &lang = _cfg.language.getAsString();
        ImWchar max = 0;
        ImWchar min = std::numeric_limits<ImWchar>::max();

        for (auto i = VcppBits::Translation::Ids{};
             i < VcppBits::Translation::Ids::_ILLEGAL_ELEMENT_;
             i = static_cast<VcppBits::Translation::Ids>(static_cast<size_t>(i) + 1)) {
            imrange_merge_string(max, min, _tr->get(i, lang));

        }
        ranges.push_back(min);
        ranges.push_back(max);

        auto default_ranges = io.Fonts->GetGlyphRangesDefault();
        size_t i = 0;
        while (default_ranges[i] != 0) {
            ranges.push_back(default_ranges[i++]);
        }

        ranges.push_back(0);

        GetSubsystem<Urho3D::SystemUI>()->AddFont("Data/Fonts/DroidSansMono.ttf",
                                                  ranges.data(),
                                                  18,
                                                  true);
    }
    _reloadLanguageRequested = false;
}
