// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "TreeEditState.hpp"

#include <limits>
#include <filesystem>


#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>


#include <VcppBits/StringUtils/StringUtils.hpp>
#include <VcppBits/MathUtils/MathUtils.hpp>
#include <VcppBits/StateManager/StateManager.hpp>


#include "TranslationIds.hpp"
#include <VcppBits/Translation/Translation.hpp>


#include "UrhoToGltf.hpp"
#include "AppSettings.hpp"

namespace fs = std::filesystem;
using namespace VcppBits::Translation;

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

inline void loadTreePresetsList (Urho3D::Context *pContext,
                                 Urho3D::StringVector &pPresets)  {
    pPresets.clear();
    Urho3D::StringVector all_inis;
    // somehow if we put "*.tree.ini" as a filter, it still gets all ".ini" files :/
    pContext->GetSubsystem<Urho3D::FileSystem>()->ScanDir(all_inis,
                                                          "Data/Presets/",
                                                          "*.ini",
                                                          Urho3D::SCAN_FILES,
                                                          false);
    ea::string must_end_with = ".tree.ini";
    for (const auto &el : all_inis) {
        if (el.substr(el.size() - must_end_with.size()) == must_end_with) {
            pPresets.push_back(el);
        }
    }
}


TreeEditState::TreeEditState (Urho3D::Context* pContext,
                              VcppBits::StateManager* pStateMgr,
                              UrhoBits::InputManager* pInputMgr,
                              AppSettings* pSettings,
                              Translation *pTranslation)
    : Urho3D::Object(pContext), // only needed for SystemUI code to subscribe to
                        // events?
    _stateMgr(pStateMgr),
    _inputMgr(pInputMgr),
    _cfg(pSettings),
      _treeConfigCache(_treeSettings),
      _translation (pTranslation) {

    _tree.setConfig(_treeConfigCache.getTreeConfig());


    for (auto categ : _treeSettings) {
        for (auto& set : categ) {
            if (set.first == "material.leaf_texture_name") {
                using std::placeholders::_1;
                set.second->addUpdateHandler<V2::StringValue>(
                    this,
                    std::bind(&TreeEditState::treeLeafTextureSettingUpdated,
                        this,
                        _1));
            }
            else {
                set.second->addSimpleUpdateHandler(
                    this,
                    std::bind(&TreeEditState::treeSettingUpdated, this));
            }
        }
    }
}

void TreeEditState::treeLeafTextureSettingUpdated (
        const std::string& pNewTextureName) {
    const ea::string new_path =
        ea::string("Tree/") + ea::string(pNewTextureName.c_str());
    auto cache = GetSubsystem<Urho3D::ResourceCache>();
    auto mat = cache->GetResource<Urho3D::Material>("Tree/Leaf.xml");
    if (cache->GetResource<Urho3D::Texture2D>(new_path)) {
        mat->SetTexture(Urho3D::TU_DIFFUSE,
            cache->GetResource<Urho3D::Texture2D>(new_path));
    }
}

void TreeEditState::treeSettingUpdated () {
    if (!_pauseUpdates && _tree.isInitialized()) {
        doReloadTree();
    }
}

void TreeEditState::processExitPressed () {
    _stateMgr->popState();
}

void TreeEditState::loadActions () {
    _actions.emplace_back(_inputMgr,
        Urho3D::KEY_ESCAPE,
        std::bind(&TreeEditState::processExitPressed, this));
}

void TreeEditState::setCameraNode (Urho3D::Node* pCameraNode) {
    _cameraNode = pCameraNode;
    _scene = _cameraNode->GetScene();
    _camCtl.init(_cameraNode);
}

void TreeEditState::reloadPresets () {
    _presets.clear();
    loadTreePresetsList(context_, _presets);

    auto sett = _cfg->tree_preset.get<V2::StringValue>();
    if (sett.size() == 0
        || findPreset(ea::string(sett.c_str())) == _presets.end()) {
        if (_presets.size()) {
            _cfg->tree_preset.set<V2::StringValue>(_presets[0].c_str());
        }
        else {
            const char* unnamed = "unnamed.tree.ini";
            _cfg->tree_preset.set<V2::StringValue>(unnamed);
            _presets.push_back(ea::string(unnamed));
        }
    }
}

void TreeEditState::reloadConf (bool pSave) {
    if (!fs::is_directory("Data/Presets")
        && !fs::create_directories("Data/Presets")) {
        throw std::runtime_error("couldn't create Data/Presets/ directory");
    }

    _pauseUpdates = true;
    if (pSave) {
        _treeSettings.writeFile();
    }
    _treeSettings.setFilename("Data/Presets/" + _cfg->tree_preset.get<V2::StringValue>());
    _treeSettings.resetAll();
    _treeSettings.load();

    _pauseUpdates = false;
}

void TreeEditState::doReloadTree () {
    Urho3D::Timer tm;

    if (!_tree.isInitialized()) {
        _tree.init(_scene, true);
    }
    else {
        _tree.regenerate();
    }

    _statsMsecs = tm.GetMSec(false);
    onModelUpdated();
}

std::string TreeEditState::getNextFreePresetName () {
    char ret[16] = "unnamed000";

    for (size_t i = 0;; ++i) {
        if (i < 1000) {
            snprintf(ret + 7, 4, "%03zu", i);
        }
        else {
            std::string meah = "unnamed" + VcppBits::StringUtils::toString(i);
            strncpy(ret, meah.c_str(), 15);
        }
        if (findPreset(ea::string(ret) + ".tree.ini") == _presets.end()) {
            return std::string(ret);
        }
    }
}

void TreeEditState::buildTree () {
    reloadPresets();
    URHO3D_LOGINFO(ea::string("selected ") +
        _cfg->tree_preset.get<V2::StringValue>().c_str());

    reloadConf();
    doReloadTree();
}

void TreeEditState::load() {
    loadActions();

    buildTree();
    loadStaticModel(_cameraNode->GetScene(), "Misc/Plane.mdl")
        ->SetScale(Urho3D::Vector3(30, 30, 30));
}

void TreeEditState::resume () {
    loadActions();
}

void TreeEditState::freeze (IState*) {
    _actions.clear();
}

void TreeEditState::unload () {
    _actions.clear();
}

void TreeEditState::update (const float pTimeStep) {
    ImGuiContext& g = *GImGui;

    auto input = GetSubsystem<Urho3D::Input>();
    if (!g.IO.WantCaptureMouse) {
        if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
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

void TreeEditState::updateModelStats () {
    _statsTree = getModelStats(_tree.getModels().first.Get());
    _statsLeaves = getModelStats(_tree.getModels().second.Get());
    _statsSum = { _statsTree.vertices + _statsLeaves.vertices,
        _statsTree.triangles + _statsLeaves.triangles };
    _statsNumLeaves = _tree.getNumLeaves();
}

void TreeEditState::exportModel () {
    auto models = _tree.getModels();

    fx::gltf::Document exp;
    appendModelToGltfDocument(exp, models.first);
    appendModelToGltfDocument(exp, models.second);

    fx::gltf::Save(exp, "export" + getTimestamp() + ".glb", true);
}

#ifndef NDEBUG
void TreeEditState::runBenchmark () {
    auto orig_seed = _treeConfigCache.getTreeConfig()->seed;
    for (decltype(orig_seed) i = 1; i < 100; ++i) {
        _treeConfigCache.getTreeConfig()->seed = i;
        _tree.regenerate();
    }

    _treeConfigCache.getTreeConfig()->seed = orig_seed;

    doReloadTree();
}
#endif // NDEBUG

void TreeEditState::renderUi () {
    TranslationBinder bnd(*_translation,
                          _cfg->language_);

    static bool demo_open = false;
    ui::SetNextWindowSize(ImVec2(200, 200),
        ImGuiCond_FirstUseEver);
    ui::SetNextWindowPos(ImVec2(100,
        100),
        ImGuiCond_FirstUseEver);

    if (ui::Begin((bnd.get(Ids::TREE) + "###Tree").c_str(), 0, 0)) {
        if (ui::Button(bnd.get(Ids::EXPORT).c_str())) {
            exportModel();
        }

#ifndef NDEBUG
        if (ui::Button(bnd.get(Ids::BENCHMARK).c_str())) {
            runBenchmark();
        }
#endif // NDEBUG


        if (_isForking) {
            renderForkingRenamingUi(true, bnd);
        }
        else if (_isRenaming) {
            renderForkingRenamingUi(false, bnd);
        }
        else {
            ImGui::PushItemWidth(160);
            renderPresetUi(bnd);
            ImGui::PopItemWidth();
        }


        if (_tree.isInitialized()) {
            // ImGui::PushItemWidth(-13 // 13 is scrollbar width
            //                          // assuming font is 2 to 1 (height to width):
            //     + ImGui::GetFontSize() / 2.f
            //     * -(float(_longestSettingLength) * 1.05f));

            // TODO how to make the right width? :/ Just give up and remove the
            // whole _longestSettingLength abracadabra?
            ImGui::PushItemWidth(120);
            render_settings2_ui(&bnd, &_treeSettings, &_longestSettingLength);
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

void TreeEditState::renderForkingRenamingUi (
        bool pForking,
        TranslationBinder &pTrBind) {
    static char preset_name[255];
    auto dest_len = static_cast<size_t>(IM_ARRAYSIZE(preset_name));
    std::string new_name;

    const auto old_path = _cfg->tree_preset.get<V2::StringValue>();
    const auto old_full_path = "Data/Presets/" + old_path;
    const auto old_name = old_path.substr(0, old_path.size() - 9);

    if (pForking) {
        new_name = getNextFreePresetName();
    }
    else {
        new_name = old_name;
    }

    if (new_name.size() >= dest_len) {
        new_name = "unnamed";
    }

    strncpy(preset_name, new_name.c_str(), new_name.size());

    ui::InputText(pTrBind.get(Ids::NEW_PRESET_NAME).c_str(),
                  preset_name,
                  dest_len);
    if (ui::IsItemDeactivated()) {
        if (findPreset(ea::string(preset_name) + ".tree.ini")
            != _presets.end()) {
            // TODO log error
        }
        else {
            _presets.push_back(ea::string(preset_name) + ".tree.ini");
            // TODO refactor and use "setPreset()" function
            _cfg->tree_preset.set<V2::StringValue>(_presets.back().c_str());
            // for now we just write what we had, instead of asking save/no?
            // should add some versioning mechanism for each tree tho...
            if (_isForking) {
                // no need to save file on rename
                _treeSettings.writeFile();
            }

            if (_isRenaming) {
                auto it = findPreset(old_path.c_str());
                if (it != _presets.end()) {
                    _presets.erase(it);
                }
                std::filesystem::rename(
                    std::filesystem::path(old_full_path),
                    std::filesystem::path("Data/Presets/" + _cfg->tree_preset.get<V2::StringValue>()));
            }

            _treeSettings.setFilename("Data/Presets/"
                                      + _cfg->tree_preset.get<V2::StringValue>());

            reloadConf();
            doReloadTree();
        }

        _isForking = false;
        _isRenaming = false;
    }
}

void TreeEditState::renderPresetUi (TranslationBinder& pTrBind) {
    static const char* curr_preset;
    curr_preset = _cfg->tree_preset.get<V2::StringValue>().c_str();
    if (ui::BeginCombo(pTrBind.get(Ids::PRESET).c_str(),
                       curr_preset,
                       0)) {
        for (size_t i = 0; i < _presets.size(); ++i) {
            const bool is_selected = (curr_preset == _presets[i]);
            if (ui::Selectable(_presets[i].c_str(), is_selected)) {
                _cfg->tree_preset.set<V2::StringValue>(_presets[i].c_str());
                curr_preset = _cfg->tree_preset.get<V2::StringValue>().c_str();
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
    if (ui::Button(pTrBind.get(Ids::FORK).c_str())) {
        _isForking = true;
    }
    ui::SameLine();
    if (ui::Button(pTrBind.get(Ids::RENAME).c_str())) {
        _isRenaming = true;
    }
    ui::SameLine();
    if (ui::Button("x")) {
        try {
            std::filesystem::create_directories("trash");
        }
        catch (std::exception& e) {
            URHO3D_LOGINFO(e.what());
        }

        const auto old_path = _cfg->tree_preset.get<V2::StringValue>();

        std::filesystem::rename(
            std::filesystem::path("Data/Presets/" + old_path),
            std::filesystem::path("trash/" + old_path));

        auto it = findPreset(getCurrentPreset());
        if (it != _presets.end()) {
            auto switch_to = getAnotherPreset(it);
            _presets.erase(it);
            if (switch_to > it) {
                ea::advance(switch_to, -1);
            }
            _cfg->tree_preset.set<V2::StringValue>(switch_to->c_str());
            reloadConf(false);
            doReloadTree();
        }
    }
}

// is used to switch onto something when we delete current
ea::vector<ea::string>::iterator
TreeEditState::getAnotherPreset (ea::vector<ea::string>::iterator pCurrent) {
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

ea::vector<ea::string>::iterator
TreeEditState::findPreset (const ea::string& pPresetName) {
    return ea::find(_presets.begin(), _presets.end(), pPresetName);
}

ea::string TreeEditState::getCurrentPreset () {
    return ea::string(_cfg->tree_preset.get<V2::StringValue>().c_str());
}

void TreeEditState::onModelUpdated () {
    updateModelStats();
    _cameraNode->LookAt(_tree.getTruncBoundingBox().Center());
    _camCtl.setPointOfInterest(_tree.getTruncBoundingBox().Center());
}

void TreeEditState::renderStats () {
    if (ui::Begin("FPS", 0, 0)) {
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
