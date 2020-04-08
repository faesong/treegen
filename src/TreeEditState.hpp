#pragma once

#include <limits>

#include <Urho3D/SystemUI/Console.h>
#include <Urho3D/SystemUI/SystemUI.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/SystemUI/SystemMessageBox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Timer.h>

#include <VcppBits/MathUtils/MathUtils.hpp>
#include <VcppBits/StringUtils/StringUtils.hpp>
#include <VcppBits/StateManager/IState.hpp>
#include <VcppBits/Settings/Settings.hpp>
#include <VcppBits/Settings/SettingsException.hpp>

#include <UrhoBits/TpsCameraController/TpsCameraController.hpp>
#include <UrhoBits/UrhoAppFramework/IUpdatedState.hpp>

#include <fx/gltf.h>

#include "AppSettings.hpp"
#include "UrhoToGltf.hpp"
#include "TreeConfigCache.hpp"

// TODO: move to UrhoUtils?
Urho3D::Node* loadStaticModel (Urho3D::Node* pParent,
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

std::string getTimestamp()
{
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

TreeStats getModelStats (Urho3D::Model* pModel) {
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
                   VcppBits::StateManager* pStateMgr,
                   UrhoBits::InputManager *pInputMgr,
                   AppSettings *pSettings)
        : Object (pContext), // only needed for SystemUI code to subscribe to
                             // events?
          _stateMgr (pStateMgr),
          _inputMgr (pInputMgr),
          _cfg (pSettings),
          _treeConfigCache (_treeSettings) {

        _tree.setConfig(_treeConfigCache.getTreeConfig());

        auto it = _treeSettings.getSettingsIterator();
        while (it.isElement()) {
            it.getCurrent().addUpdateHandler(
                this,
                std::bind(&TreeEditState::treeSettingUpdated, this));

            it.peekNext();
        }
    }

    void treeSettingUpdated () {
        if (!_pauseUpdates && _tree.isInitialized()) {
            _tree.regenerate();
        }
    }

    void processExitPressed () {
        _stateMgr->popState();
    }

    void loadActions () {
        _actions.emplace_back(_inputMgr,
                              Urho3D::KEY_ESCAPE,
                              std::bind(&TreeEditState::processExitPressed, this));
    }

    void setCameraNode (Urho3D::Node *pCameraNode) {
        _cameraNode = pCameraNode;
        _scene = _cameraNode->GetScene();
        _camCtl.init(_cameraNode);
    }

    void reloadPresets () {
        _presets.clear();
        UrhoBits::loadTreePresetsList(context_, _presets);

        auto sett = _cfg->tree_preset.getString();
        if (sett.size() == 0
            || findPreset(ea::string(sett.c_str())) == _presets.end()) {
            if (_presets.size()) {
                _cfg->tree_preset.setString(_presets[0].c_str());
            } else {
                const char* unnamed = "unnamed.tree.ini";
                _cfg->tree_preset.setString(unnamed);
                _presets.push_back(ea::string(unnamed));
            }
        }
    }

    void doReloadTree (){
        _pauseUpdates = true;

        _treeSettings.writeFile();
        _treeSettings.setFilename(_cfg->tree_preset.getString());
        _treeSettings.resetAll();
        _treeSettings.load();

        _pauseUpdates = false;

        Urho3D::Timer tm;

        if (!_tree.isInitialized()) {
            _tree.init(_scene, true);
        } else {
            _tree.regenerate();
        }

        _statsMsecs = tm.GetMSec(false);
        onModelUpdated();
    }

    std::string getNextFreePresetName () {
        char ret[16] = "unnamed000";

        for (size_t i = 0;; ++i) {
            if (i < 1000) {
                snprintf (ret + 7, 4, "%03zu", i);
            } else {
                std::string meah = "unnamed" + VcppBits::StringUtils::toString(i);
                strncpy(ret, meah.c_str(), 15);
            }
            if (findPreset(ea::string(ret) + ".tree.ini") == _presets.end()) {
                return std::string(ret);
            }
        }
    }

    void buildTree () {
        reloadPresets();
        URHO3D_LOGINFO(ea::string("selected ") +
                       _cfg->tree_preset.getString().c_str());

        doReloadTree();
    }


    void load () override {
        loadActions();

        buildTree();
        loadStaticModel(_cameraNode->GetScene(), "Misc/Plane.mdl")
            ->SetScale(Urho3D::Vector3(30, 30, 30));

        _cameraNode->LookAt(_tree.getTruncBoundingBox().Center());
    }
    void resume () override {
        loadActions();
    }
    void freeze (IState * /*pState*/ ) override {
        _actions.clear();
    }
    void unload () override {
        _actions.clear();
    }
    std::string getName () const override { return "MainMenu"; }

    void update (const float pTimeStep) override {
        ImGuiContext& g = *GImGui;

        auto input = GetSubsystem<Urho3D::Input>();
        if (!g.IO.WantCaptureMouse) {
            if(input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
                _camCtl.update(pTimeStep,
                               input->GetMouseMove());
            }
            auto wheelmove = input->GetMouseMoveWheel();
            if (wheelmove) {
                _camCtl.zoom(1.f - VcppBits::MathUtils::sign(wheelmove) * 0.1f);
            }
        }
        renderUi();

        ++_framesPassed;
        _frameTimesPassed += pTimeStep;

        if (_frameTimesPassed >= 1.f) {
            _fps = float(_framesPassed) / _frameTimesPassed;
        }
    }

private:
    void updateModelStats () {
        _statsTree = getModelStats(_tree.getModels().first.Get());
        _statsLeaves = getModelStats(_tree.getModels().second.Get());
        _statsSum = { _statsTree.vertices + _statsLeaves.vertices,
                      _statsTree.triangles + _statsLeaves.triangles };
        _statsNumLeaves = _tree.getNumLeaves();
    }

    void exportModel () {
        auto models = _tree.getModels();

        fx::gltf::Document exp;
        appendModelToGltfDocument(exp, models.first);
        appendModelToGltfDocument(exp, models.second);


        fx::gltf::Save(exp, "export" + getTimestamp() + ".glb", true);
    }

    void exportUrhoModel () {
        auto models = _tree.getModels();
        const auto ts = getTimestamp();
        models.first->SaveFile(ea::string("trunc-") + getTimestamp().c_str() + ".mdl");
        models.second->SaveFile(ea::string("leaves-") + getTimestamp().c_str() + ".mdl");
    }


    void renderUi () {
        static bool demo_open = false;
        ui::SetNextWindowSize(ImVec2(200, 200),
                              ImGuiCond_FirstUseEver);
        ui::SetNextWindowPos(ImVec2(100,
                                    100),
                             ImGuiCond_FirstUseEver);
        if (ui::Begin("Tree Settings", 0, 0)) {
            if (ui::Button("Export")) {
                exportModel();
            }
            if (ui::Button("ExportUrho")) {
                exportUrhoModel();
            }

            if (messageBox_) {
                if (ui::Button("Close message box")) {
                    messageBox_ = nullptr;
                }
            }
            else {
                if (ui::Button("Show message box")) {
                    messageBox_ =
                        new Urho3D::SystemMessageBox(context_,
                                                     "Hello from SystemUI",
                                                     "Sample Message Box");
                    SubscribeToEvent(
                        Urho3D::E_MESSAGEACK,
                        [&](Urho3D::StringHash, Urho3D::VariantMap&) {
                            messageBox_ = nullptr;
                        });
                }
            }

            if (ui::Button("Toggle console")) {
                //GetSubsystem<Console>()->Toggle();
            }
            if (ui::Button("Toggle demo window"))
                demo_open ^= true;

            if (_isForking) {
                renderForkingUi();
            } else {
                renderPresetUi();
            }


            if (_tree.isInitialized()) {
                ImGui::PushItemWidth(-13 // 13 is scrollbar width
                                     // assuming font is 2 to 1 (height to width):
                                     + ImGui::GetFontSize()/2.f 
                                     * -(float(_longestSettingLength) * 1.05f));
                renderTreeSettingsUi();
                ImGui::PopItemWidth();
            }
        }
        //updateWindowTransforms(ui::GetWindowPos(), ui::GetWindowSize());
        ui::End();

        renderStats();
        if (demo_open) {
            ui::ShowDemoWindow(&demo_open);
        }
    }

    void renderForkingUi () {
        static char preset_name[15];
        auto dest_len = static_cast<size_t>(IM_ARRAYSIZE(preset_name));
        auto next_free_preset_name = getNextFreePresetName();
        if (next_free_preset_name.size() >= dest_len) {
            next_free_preset_name = "unnamed";
        }
        strncpy(preset_name, next_free_preset_name.c_str(), next_free_preset_name.size());
        ui::InputText("New preset name",
                      preset_name,
                      dest_len);
        if (ui::IsItemDeactivated()) {
            if (findPreset(ea::string(preset_name) + ".tree.ini")
                != _presets.end()) {
                // TODO log error
            } else {
                _presets.push_back(ea::string(preset_name) + ".tree.ini");
                // TODO refactor and use "setPreset()" function
                _cfg->tree_preset.setString(_presets.back().c_str());

                doReloadTree();
            }
            _isForking = false;
        }
    }

    void renderPresetUi () {
        static const char *curr_preset;
        curr_preset = _cfg->tree_preset.getString().c_str();
        if (ui::BeginCombo("Preset", curr_preset, 0)) {
            for (size_t i = 0; i < _presets.size(); ++i) {
                const bool is_selected = (curr_preset == _presets[i]);
                if (ui::Selectable(_presets[i].c_str(), is_selected)) {
                    _cfg->tree_preset.setString(_presets[i].c_str());
                    curr_preset = _cfg->tree_preset.getString().c_str();
                    _treeSettings.writeFile();
                    buildTree();
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ui::EndCombo(); 
        }
        ui::SameLine();
        if (ui::Button("fork")) {
            _isForking = true;
        }
        ui::SameLine();
        if (ui::Button("rename")) {
            // TODO
        }
        ui::SameLine();
        if (ui::Button("x")) {
            auto it = findPreset(getCurrentPreset());
            if (it != _presets.end()) {
                auto switch_to = getAnotherPreset(it);
                _presets.erase(it);
                if (switch_to > it) {
                    ea::advance(switch_to, -1);
                }
                _cfg->tree_preset.setString(switch_to->c_str());
                doReloadTree();
            }
        }
    }

    // is used to switch onto something when we delete current
    ea::vector<ea::string>::iterator
    getAnotherPreset (ea::vector<ea::string>::iterator pCurrent) {
        auto ret = pCurrent;
        if (_presets.size() > 1) {
            ea::advance(ret, 1);
            if (ret == _presets.end()) {
                ea::advance(ret, -2);
            }
            return ret;
        }
        _presets.push_back("default.tree.ini");
        ret = _presets.end();
        eastl::advance(ret, -1);
        return ret;
    }

    ea::vector<ea::string>::iterator findPreset (const ea::string& pPresetName) {
        return ea::find(_presets.begin(), _presets.end(), pPresetName);
    }

    ea::string getCurrentPreset () {
        return ea::string(_cfg->tree_preset.getString().c_str());
    }

    void renderSettingUi (VcppBits::Setting& pSetting, int x_id) {
        auto& set = pSetting; // todo shortcut/refactor

        ui::PushID(x_id);
        if (ui::Button("x")) {
            set.resetToDefault();
        }
        ui::PopID();
        ui::SameLine();

        const auto curr_set_text =
            ea::string(set.getAsString().c_str());
        // x,y,z,w
        //ImVec4 tint = GetStyleColorVec4
        //ui::PushStyleColor(ImGuiCol_FrameBg,
        bool is_default = set.isDefault();
        if(is_default) {
            ui::PushStyleVar(ImGuiStyleVar_Alpha, .45f);
        }
      //  _tree.pauseUpdates();
        switch (set.getType()) {
        case VcppBits::Setting::TYPE::S_BOOL:
            if (ui::Checkbox(set.getName().c_str(),
                             set.getBoolPtr())) {
                clampSetting(set);
            }
            break;
        case VcppBits::Setting::TYPE::S_INTEGER:
            if (ui::InputInt(set.getName().c_str(),
                             set.getIntPtr(),
                             0, // step?
                             0)) { // stepfast??
                clampSetting(set);
            }
            break;
        case VcppBits::Setting::TYPE::S_INTEGER_BOUNDED:
            if (ui::DragInt(set.getName().c_str(),
                            set.getIntPtr(),
                            0.1f, // TODO figure out proper speed
                            set.getIntDown(),
                            set.getIntUp())) {
                clampSetting(set);
            }
            break;
        case VcppBits::Setting::TYPE::S_FLOATINGPOINT:
            if (ui::InputFloat(set.getName().c_str(),
                               set.getFloatPtr(),
                               0.025f,
                               1.0f)) {
                clampSetting(set);
            }
            break;
        case VcppBits::Setting::TYPE::S_FLOATINGPOINT_BOUNDED:
            if (ui::DragFloat(set.getName().c_str(),
                              set.getFloatPtr(),
                              0.025f, // TODO figure out proper speed
                              set.getFloatDown(),
                              set.getFloatUp())) {
                clampSetting(set);
            }
            break;
        default:
            ui::Text("%s", curr_set_text.c_str());
        }
        // TODO: don't do this every damn frame..
        if (set.getName().size() > _longestSettingLength) {
            _longestSettingLength = set.getName().size();
        }
        if (is_default) {
            ui::PopStyleVar(1);
        }
    }

    void clampSetting(VcppBits::Setting& pSetting) {
        Urho3D::Timer tm;
        switch (pSetting.getType()) {
        case VcppBits::Setting::TYPE::S_INTEGER_BOUNDED:
            pSetting.setInt(VcppBits::MathUtils::clamp(pSetting.getInt(),
                                                       pSetting.getIntDown(),
                                                       pSetting.getIntUp()));
            break;
        case VcppBits::Setting::TYPE::S_FLOATINGPOINT_BOUNDED:
            pSetting.setFloat(VcppBits::MathUtils::clamp(pSetting.getFloat(),
                                                         pSetting.getFloatDown(),
                                                         pSetting.getFloatUp()));
            break;
        case VcppBits::Setting::TYPE::S_STRING_ONE_OF:
            try {
                pSetting.setString(pSetting.getString());
            } catch (const VcppBits::SettingsException&) {
                // revert to what it was
                pSetting.setString(
                    pSetting.getPossibleStrings()[pSetting.getStringPos()]);
            }
            break;
        default:
            // do nothing otherwise
            break;
        }
        if (0)
        for (int i = 0; i < 200; ++i) {
            pSetting.setInt(pSetting.getInt());
        }
        _statsMsecs = tm.GetMSec(false);
        onModelUpdated();
    }

    void onModelUpdated () {
        updateModelStats();
        _camCtl.setPointOfInterest(_tree.getTruncBoundingBox().Center());
    }

    void renderTreeSettingsUi () {
        int i = 0;
        for (auto categ : _treeSettings) {
            //URHO3D_LOGINFO(categ.getName().c_str());

            std::string cat_name = categ.getName();

            if (!cat_name.size()) {
                cat_name = "General";
            }

            if (ui::CollapsingHeader(cat_name.c_str())) {
                for (auto &set : categ) {
                    renderSettingUi(set, i);
                    ++i;
                }
            }
        }
    }

    void renderStats () {
        if (ui::Begin("Fps", 0, 0)) {
            ImGui::Text("%f", _fps);
        }
        ui::End();
        if (ui::Begin("Stats", 0, 0)) {
            ImGui::Text("Generated in (msecs):");
            ImGui::SameLine();
            ImGui::Text("%d", _statsMsecs);

            ImGui::Text("  Tree | Triangles: %ld Vertices: %ld",
                        _statsTree.triangles,
                        _statsTree.vertices);
            ImGui::Text("Leaves | Triangles: %ld Vertices: %ld",
                        _statsLeaves.triangles,
                        _statsLeaves.vertices);
            ImGui::Text(" Total | Triangles: %ld Vertices: %ld",
                        _statsSum.triangles,
                        _statsSum.vertices);
            ImGui::Text("Leaves: %ld", _statsNumLeaves);

        }
        ui::End();
    }

    bool _pauseUpdates = false;

    Urho3D::SharedPtr<Urho3D::SystemMessageBox> messageBox_;

    VcppBits::StateManager *_stateMgr;
    UrhoBits::InputManager *_inputMgr;

    std::deque<UrhoBits::InputManagerAction> _actions;

    Urho3D::Node *_cameraNode = nullptr;
    Urho3D::Scene *_scene = nullptr;
    UrhoBits::TreeGenerator _tree;

    UrhoBits::TpsCameraController _camCtl;

    AppSettings *_cfg = nullptr; // TODO move

    VcppBits::Settings _treeSettings;
    TreeConfigCache _treeConfigCache;

    ea::vector<ea::string> _presets;

    unsigned _statsMsecs = 0;
    TreeStats _statsTree;
    TreeStats _statsLeaves;
    TreeStats _statsSum;
    size_t _statsNumLeaves = 0;
    size_t _longestSettingLength = 5;

    bool _isForking = false;

    size_t _framesPassed = 0;
    float _frameTimesPassed = 0.f;
    float _fps = 0.f;
};
