#include <VcppBits/StateManager/IState.hpp>
#include <VcppBits/StateManager/StateManager.hpp>

#include "SettingsUi.hpp"
#include "AppSettings.hpp"

class RootState final : public UrhoBits::IUpdatedState {
public:
    RootState (VcppBits::StateManager *pStateManager,
        AppSettings *pSettings)
        : _stateMgr(pStateManager),
          _settings (pSettings) {

    }

    bool alwaysUpdate () override { return true; }

    void load () override {
    }
    void resume () override {
        _stateMgr->popState();
    }

    void freeze (IState*) override {

    }

    void unload () override {

    }

    std::string getName () const override { return "RootState"; }

    void update (const float /*pTimeStep*/) override {
        renderSettingsUi();
    }

    void renderSettingsUi () {
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

        if (ui::Begin("Settings", 0, 0)) {
            render_settings_ui (&_settings->_settings, nullptr);
        }
        ui::End();

        ImGui::End();
    }

private:
    VcppBits::StateManager *_stateMgr;
    AppSettings *_settings;
};
