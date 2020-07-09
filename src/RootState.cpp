// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include "TranslationIds.hpp"
#include <VcppBits/Translation/Translation.hpp>


#include "RootState.hpp"


#include "AppSettings.hpp"
#include "SettingsUi.hpp"


RootState::RootState (const VcppBits::Translation::Translation* pTranslation,
                      VcppBits::StateManager *pStateManager,
                      AppSettings *pSettings)
    : _stateMgr (pStateManager),
      _settings (pSettings),
      _translation (pTranslation) {
}

void RootState::load () {
}

void RootState::resume () {
    _stateMgr->popState();
}

void RootState::freeze (IState*) {
}

void RootState::unload () {
}

void RootState::update (const float /*pTimeStep*/) {
    renderDockSpaceUi();
    renderSettingsUi();
}

void RootState::renderDockSpaceUi () {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::Begin("DockSpace",
                 nullptr,
                 ImGuiWindowFlags_NoBackground
                 |ImGuiWindowFlags_NoBringToFrontOnFocus
                 | ImGuiWindowFlags_NoInputs
                 // TODO5: there has to be either title or menu bar
                 // otherwise docking doesn't work as intended
                 // (try to update IMGUI?)
                 | ImGuiWindowFlags_MenuBar
                 | ImGuiWindowFlags_NoTitleBar);


    static ImGuiDockNodeFlags dockspace_flags =
        ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    ui::PopStyleVar(); // rounding
    ui::PopStyleVar(); // padding

    ImGui::End();
}

void RootState::renderSettingsUi () {
    if (ui::Begin("Settings", 0, 0)) {
        VcppBits::Translation::TranslationBinder bnd(*_translation, _settings->language_);
        render_settings2_ui (&bnd, &_settings->_settings, nullptr);
    }
    ui::End();
}
