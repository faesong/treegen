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
        if (ui::Begin("Settings!!!", 0, 0)) {
            render_settings_ui (&_settings->_settings, nullptr);
        }
        ui::End();
    }

private:
    VcppBits::StateManager *_stateMgr;
    AppSettings *_settings;
};
