#pragma once

#include <UrhoBits/TpsCameraController/TpsCameraController.hpp>
#include <UrhoBits/UrhoAppFramework/IUpdatedState.hpp>
#include <UrhoBits/InputManager/InputManager.hpp>


#include <fx/gltf.h>

#include "UrhoToGltf.hpp"
#include "TreeConfigCache.hpp"
#include "SettingsUi.hpp"

namespace Urho3D {
class Node;
class Scene;
}

struct AppSettings;

namespace VcppBits {
class StateManager;

namespace Translation {
class Translation;
class TranslationBinder;
}
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
    using Translation = VcppBits::Translation::Translation;
    using TranslationBinder = VcppBits::Translation::TranslationBinder;

    TreeEditState (Urho3D::Context *pContext,
                   VcppBits::StateManager *pStateMgr,
                   UrhoBits::InputManager *pInputMgr,
                   AppSettings *pSettings,
                   Translation *pTranslation);

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

#ifndef NDEBUG
    void runBenchmark ();
#endif // NDEBUG

    void renderUi ();

    void renderForkingRenamingUi (bool pForking,
                                  TranslationBinder &pTrBind);

    void renderPresetUi (TranslationBinder& pTrBind);

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

    Translation *_translation;
};
