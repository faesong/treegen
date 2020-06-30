#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Math/Vector3.h>

namespace Urho3D {
class Scene;
class Node;
class RenderPath;
class Zone;
class Light;
}


#include <VcppBits/Translation/Translation.hpp>

#include <UrhoBits/UrhoAppFramework/UrhoAppFramework.hpp>


#include "TreeGenSettings.hpp"
#include "RootState.hpp"
#include "TreeEditState.hpp"
#include "AppSettings.hpp"


class TreeGen : public UrhoBits::UrhoAppFramework {
    URHO3D_OBJECT(TreeGen, UrhoAppFramework);
public:
    TreeGen (Urho3D::Context* pContext);

    void onLanguageSettingUpdate (const std::string &pNewLang);
    void onAmbientSettingUpdate (const Urho3D::Vector3 &pAmbient);
    void onLightDirectionSettingUpdate (const Urho3D::Vector3 &pLightDir);
    void onFogColorSettingUpdate (const Urho3D::Vector3 &pFogColor);
    void onShadowIntensitySettingUpdate (const float &pIntensity);
    void onAutoExposureSettingUpdate (const bool &pAutoExposure);
    void onFxaaSettingUpdate (const bool &pFxaa);
    void onSsaoSettingUpdate (const float pSsao);
    void onVibranceSettingUpdate (const float pVibrance);

    void initPostProcessingSettings ();

    void setup () override;
    void start () override;
    void endFrame () override;

    void setupScene ();

private:
    VcppBits::Translation::Translation _tr;
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

    bool _reloadLanguageRequested = true;
};
