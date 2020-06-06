#pragma once

#include <ImGui/imgui_stdlib.h>

void clampSetting (VcppBits::Setting& pSetting) {
    Urho3D::Timer tm;
    switch (pSetting.getType()) {
    case VcppBits::Setting::TYPE::S_BOOL:
        pSetting.setBool(pSetting.getBool());
        break;
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

    // TODO
    //_statsMsecs = tm.GetMSec(false);
    //onModelUpdated();
}

void render_setting_ui (VcppBits::Setting& pSetting,
                        int x_id) {
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

    if (is_default) {
        ui::PopStyleVar(1);
    }
}


void render_settings_ui (VcppBits::Settings * pSettings,
                         size_t *pLongestSettingPtr) {
    bool calc_length = false;
    if (pLongestSettingPtr && *pLongestSettingPtr == 0) {
        calc_length = true;
    }

    int i = 0;
    for (auto categ : *pSettings) {
        //URHO3D_LOGINFO(categ.getName().c_str());

        std::string cat_name = categ.getName();

        if (!cat_name.size()) {
            cat_name = "General";
        }

        if (ui::CollapsingHeader(cat_name.c_str())) {
            for (auto &set : categ) {
                render_setting_ui(set, i);

                if (calc_length) {
                    if (set.getName().size() > *pLongestSettingPtr) {
                        *pLongestSettingPtr = set.getName().size();
                    }
                }

                ++i;
            }
        }
    }
}

// TODO switch to UrhoBits/SettingsUi
struct StdInputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int StdInputTextCallback (ImGuiInputTextCallbackData* data)
{
    StdInputTextCallback_UserData* user_data = (StdInputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen, '+');
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

inline bool StdInputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    StdInputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, StdInputTextCallback, &cb_user_data);
}


// TODO switch to UrhoBits/SettingsUi
void render_setting2_ui(std::string pName,
                        Setting2* pSetting,
                        int x_id) {
    auto& set = *pSetting; // todo shortcut/refactor

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
    if (is_default) {
        ui::PushStyleVar(ImGuiStyleVar_Alpha, .45f);
    }
    //  _tree.pauseUpdates();
    switch (set.getType()) {
    case SettingTypeEnum::BOOL:
        if (ui::Checkbox(pName.c_str(),
            set.getIncomingPtr<V2::BoolValue>())) {
            set.setFromIncomingPtr<V2::BoolValue>();
        }
        break;
    case SettingTypeEnum::INT:
        if (ui::DragInt(pName.c_str(),
                        set.getIncomingPtr<V2::IntValue>(),
                        1, // TODO figure out proper speed
                        set.getConstraint<V2::IntValue>()._min,
                        set.getConstraint<V2::IntValue>()._max)) {
            set.setFromIncomingPtr<V2::IntValue>();
        }
        break;
    case SettingTypeEnum::FLOAT:
        if (ui::DragFloat(pName.c_str(),
                          set.getIncomingPtr<V2::FloatValue>(),
                          .01f, // TODO figure out proper speed
                          set.getConstraint<V2::FloatValue>()._min,
                          set.getConstraint<V2::FloatValue>()._max)) {
            set.setFromIncomingPtr<V2::FloatValue>();
        }
        break;
    case SettingTypeEnum::VECTOR3:
        if (pName.find("_color") < pName.size()) {
            if (ui::ColorEdit3(pName.c_str(),
                (float*)set.getIncomingPtr<Vector3Value>())) {
                set.setFromIncomingPtr<Vector3Value>();
            }
        }
        else {
            if (ui::DragFloat3(pName.c_str(),
                (float*)set.getIncomingPtr<Vector3Value>(),
                .01f, // TODO figure out proper speed
                set.getConstraint<Vector3Value>()._min.x_,
                set.getConstraint<Vector3Value>()._max.x_)) {
                set.setFromIncomingPtr<Vector3Value>();
            }
        }
        break;
    case SettingTypeEnum::STRING:
    {
        auto *ptr = set.getIncomingPtr<V2::StringValue>();
        StdInputText(pName.c_str(),
            ptr,
            0,
            nullptr,
            nullptr);
        if (ui::IsItemDeactivated()) {
            // what the actual fuck?
            *ptr = ptr->c_str();
            set.setFromIncomingPtr<V2::StringValue>();
        }
        break;
    }
    case SettingTypeEnum::ENUM_STRING: {
        //const std::string& curr = set
        const auto &vals = set.getEnumElements<V2::EnumStringValue>();
        const auto &curr_val = set.get<V2::EnumStringValue>();
        if (ImGui::BeginCombo(pName.c_str(), curr_val.c_str(), 0)) {
            for (const auto &el : vals) {
                bool is_selected = (curr_val == el);
                if (ImGui::Selectable(el.c_str(), is_selected)) {
                    set.set<V2::EnumStringValue>(el);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        break;
    }
    default:
        ui::Text("%s", curr_set_text.c_str());
    }

    if (is_default) {
        ui::PopStyleVar(1);
    }
}

void render_settings2_ui(Settings2* pSettings,
    size_t* pLongestSettingPtr) {
    bool calc_length = false;
    if (pLongestSettingPtr && *pLongestSettingPtr == 0) {
        calc_length = true;
    }

    int i = 0;
    for (auto categ : *pSettings) {
        //URHO3D_LOGINFO(categ.getName().c_str());

        std::string cat_name = categ.getName();

        if (!cat_name.size()) {
            cat_name = "General";
        }

        if (ui::CollapsingHeader(cat_name.c_str())) {
            for (auto& set : categ) {
                render_setting2_ui(set.first, set.second, i);

                if (calc_length) {
                    if (set.first.size() > * pLongestSettingPtr) {
                        *pLongestSettingPtr = set.first.size();
                    }
                }

                ++i;
            }
        }
    }
}
