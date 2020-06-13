#pragma once

#include <limits>
#include <filesystem>

#include <Urho3D/SystemUI/Console.h>
#include <Urho3D/SystemUI/SystemUI.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/SystemUI/SystemMessageBox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/IO/Log.h>

#include <VcppBits/MathUtils/MathUtils.hpp>
#include <VcppBits/StringUtils/StringUtils.hpp>
#include <VcppBits/StateManager/IState.hpp>
#include <VcppBits/StateManager/StateManager.hpp>
#include <VcppBits/Settings/Settings.hpp>
#include <VcppBits/Settings/SettingsException.hpp>

#include <UrhoBits/TpsCameraController/TpsCameraController.hpp>
#include <UrhoBits/UrhoAppFramework/IUpdatedState.hpp>
#include <UrhoBits/InputManager/InputManager.hpp>

#include <fx/gltf.h>

#include "AppSettings.hpp"
#include "UrhoToGltf.hpp"
#include "TreeConfigCache.hpp"
#include "SettingsUi.hpp"

// TODO: move to UrhoUtils?
inline Urho3D::Node* loadStaticModel (Urho3D::Node* pParent,
                                      const ea::string &pModelPath,
                                      const ea::string &pMaterialPath = "",
                                      const ea::string &pNodeName = "") {
    auto node = pParent->CreateChild(pNodeName);
    auto cache = node->GetSubsystem<Urho3D::ResourceCache>();
    auto obj = node->CreateComponent<Urho3D::StaticModel>();
    obj->SetModel(cache->GetResource<Urho3D::Model>(pModelPath));
    obj->SetMaterial(cache->GetResource<Urho3D::Material>(pMaterialPath));

    return node;
}



#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string

inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y.%m.%d_%H-%M-%S");
    return ss.str();
}


struct TreeStats {
    size_t vertices{};
    size_t triangles{};
};

inline TreeStats getModelStats (Urho3D::Model* pModel) {
    TreeStats ret;

    const auto &vbuffs = pModel->GetVertexBuffers();
    for (const auto &buf : vbuffs) {
        ret.vertices += buf->GetVertexCount();
    }

    const auto &ibuffs = pModel->GetIndexBuffers();
    for (const auto &buf : ibuffs) {
        ret.triangles += buf->GetIndexCount() / 3;
    }

    const auto &geoms = pModel->GetGeometries();

    for(const auto &geoms_ : geoms) {
        for (const auto& geom : geoms_) {
            const auto& g_vbuffs = geom->GetVertexBuffers();
            for (const auto& buf : g_vbuffs) {
                ret.vertices += buf->GetVertexCount();
            }

            ret.triangles += geom->GetIndexBuffer()->GetIndexCount() / 3;
        }
    }

    return ret;
}

class TreeEditState final : public UrhoBits::IUpdatedState,
                            public Urho3D::Object {
    URHO3D_OBJECT(TreeEditState, Object);
public:
    TreeEditState (Urho3D::Context *pContext,
                   VcppBits::StateManager *pStateMgr,
                   UrhoBits::InputManager *pInputMgr,
                   AppSettings *pSettings);

    void treeLeafTextureSettingUpdated (const std::string& pNewTextureName);

    void treeSettingUpdated ();

    void processExitPressed ();

    void loadActions ();

    void setCameraNode (Urho3D::Node* pCameraNode);

    void reloadPresets ();

    void reloadConf (bool pSave = true);

    void doReloadTree ();

    std::string getNextFreePresetName ();

    void buildTree ();


    void load () override;
    void resume () override;
    void freeze (IState* /*pState*/) override;
    void unload () override;
    std::string getName () const override { return "TreeEditState"; }

    void update (const float pTimeStep) override;

private:
    void updateModelStats ();

    void exportModel ();

    void renderUi ();

    void renderForkingRenamingUi (bool pForking);

    void renderPresetUi ();

    // is used to switch onto something when we delete current
    ea::vector<ea::string>::iterator
        getAnotherPreset (ea::vector<ea::string>::iterator pCurrent);

    ea::vector<ea::string>::iterator findPreset (const ea::string& pPresetName);

    ea::string getCurrentPreset ();

    void onModelUpdated ();

    void renderStats ();

    bool _pauseUpdates = false;

    VcppBits::StateManager *_stateMgr;
    UrhoBits::InputManager *_inputMgr;

    std::deque<UrhoBits::InputManagerAction> _actions;

    Urho3D::Node *_cameraNode = nullptr;
    Urho3D::Scene *_scene = nullptr;
    UrhoBits::TreeGenerator _tree;

    UrhoBits::TpsCameraController _camCtl;

    AppSettings *_cfg = nullptr; // TODO move

    Settings2 _treeSettings;
    TreeConfigCache _treeConfigCache;

    ea::vector<ea::string> _presets;

    unsigned _statsMsecs = 0;
    TreeStats _statsTree;
    TreeStats _statsLeaves;
    TreeStats _statsSum;
    size_t _statsNumLeaves = 0;
    size_t _longestSettingLength = 0;

    bool _isForking = false;
    bool _isRenaming = false;

    size_t _framesPassed = 0;
    float _frameTimesPassed = 0.f;
    float _fps = 0.f;
};
