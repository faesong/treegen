#pragma once

#include <Urho3D/SystemUI/Console.h>
#include <Urho3D/SystemUI/SystemUI.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/SystemUI/SystemMessageBox.h>

#include "contrib/StateManager/IState.hpp"
#include "IUpdatedState.hpp"


class MainMenu final : public IUpdatedState,
                       public Urho3D::Object {
    URHO3D_OBJECT(MainMenu, Object);
public:
    MainMenu (Urho3D::Context *pContext,
              StateManager* pStateMgr,
              InputManager *pInputMgr)
        : Object (pContext), // only needed for SystemUI code to subscribe to
                             // events?
          _stateMgr (pStateMgr),
          _inputMgr (pInputMgr) {
    }

    void processExitPressed () {
        _stateMgr->popState();
    }

    void loadActions () {
        _actions.emplace_back(_inputMgr,
                              Urho3D::KEY_ESCAPE,
                              std::bind(&MainMenu::processExitPressed, this));
    }

    void load () override {
        loadActions();
    }
    void resume () override {
        loadActions();
    }
    void freeze (IState *pState) override {
        _actions.clear();
    }
    void unload () override {
        _actions.clear();
    }
    std::string getName () const override { return "MainMenu"; }

    void update (const float pTimeStep) override {
        renderUi();
    }
    void renderUi () {
        ui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
        ui::SetNextWindowPos(ImVec2(200, 300), ImGuiCond_FirstUseEver);
        if (ui::Begin("Sample SystemUI", 0, ImGuiWindowFlags_NoSavedSettings)) {
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
        }
        ui::End();
    }
private:
    Urho3D::SharedPtr<Urho3D::SystemMessageBox> messageBox_;

    StateManager *_stateMgr;
    InputManager *_inputMgr;

    std::deque<InputManagerAction> _actions;
    TreeEditState _treeEditState;
};


