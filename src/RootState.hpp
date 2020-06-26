#pragma once


#include <VcppBits/StateManager/StateManager.hpp>


#include <UrhoBits/UrhoAppFramework/IUpdatedState.hpp>


struct AppSettings;

class RootState final : public UrhoBits::IUpdatedState {
public:
    RootState (VcppBits::StateManager *pStateManager,
               AppSettings *pSettings);

    bool alwaysUpdate () override { return true; }

    void load () override;
    void resume () override;
    void freeze (IState*) override;
    void unload () override;
    std::string getName () const override { return "RootState"; }

    void update (const float /*pTimeStep*/) override;

    void renderDockSpaceUi ();
    void renderSettingsUi ();
private:
    VcppBits::StateManager *_stateMgr;
    AppSettings *_settings;
};
