#pragma once

#include "TreeGenSettings.hpp"

#include <Urho3D/SystemUI/SystemUI.h>

#include <ImGui/imgui_stdlib.h>

namespace VcppBits {
    class Setting;
    class Settings;
}

void clampSetting (VcppBits::Setting& pSetting);

void render_setting_ui (VcppBits::Setting& pSetting,
                        int x_id);


void render_settings_ui (VcppBits::Settings * pSettings,
                         size_t* pLongestSettingPtr);

// TODO switch to UrhoBits/SettingsUi
struct StdInputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

inline static int StdInputTextCallback (ImGuiInputTextCallbackData* data) {
    StdInputTextCallback_UserData* user_data =
        (StdInputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or
        // capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(size_t(data->BufTextLen), '+');
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback) {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

inline bool StdInputText (const char* label,
                          std::string* str,
                          ImGuiInputTextFlags flags,
                          ImGuiInputTextCallback callback,
                          void* user_data) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    StdInputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ui::InputText(label,
                         (char*)str->c_str(),
                         str->capacity() + 1,
                         flags,
                         StdInputTextCallback,
                         &cb_user_data);
}


// TODO switch to UrhoBits/SettingsUi
void render_setting2_ui (std::string pName,
                         Setting2* pSetting,
                         int x_id);

void render_settings2_ui (Settings2* pSettings,
                          size_t* pLongestSettingPtr);
