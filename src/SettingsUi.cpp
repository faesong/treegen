// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "SettingsUi.hpp"

#include <VcppBits/Settings/Setting.hpp>
#include <VcppBits/Settings/SettingsException.hpp>
#include <VcppBits/Settings/Settings.hpp>
#include <VcppBits/MathUtils/MathUtils.hpp>


#include "TranslationIds.hpp"
#include <VcppBits/Translation/Translation.hpp>

using Ids = VcppBits::Translation::Ids;
using TranslationBinder = VcppBits::Translation::TranslationBinder;


void clampSetting(VcppBits::Setting& pSetting) {
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
        }
        catch (const VcppBits::SettingsException&) {
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

void render_setting_ui(VcppBits::Setting& pSetting, int x_id) {
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
    if (is_default) {
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

void render_settings_ui(VcppBits::Settings* pSettings, size_t* pLongestSettingPtr) {
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
                render_setting_ui(set, i);

                if (calc_length) {
                    if (set.getName().size() > * pLongestSettingPtr) {
                        *pLongestSettingPtr = set.getName().size();
                    }
                }

                ++i;
            }
        }
    }
}

static void imgui_render_help_marker (const char* desc)
{
    ImGui::Text("(?)");
    if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
}


void render_setting2_ui (const TranslationBinder *pTr,
                         std::string pName,
                         Setting2* pSetting,
                         int &id) {
    auto& set = *pSetting; // todo shortcut/refactor

    ui::PushID(id);
    if (ui::Button("x")) {
        set.resetToDefault();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(pTr->get(Ids::SETTINGS_RESET_TO_DEFAULT).c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ui::PopID();
    ++id;
    ui::SameLine();
    const auto curr_set_text =
        ea::string(set.getAsString().c_str());
    // x,y,z,w
    //ImVec4 tint = GetStyleColorVec4
    //ui::PushStyleColor(ImGuiCol_FrameBg,
    bool is_default = set.isDefault();
    if (is_default) {
        ui::PushStyleVar(ImGuiStyleVar_Alpha, .75f);
    }
    //  _tree.pauseUpdates();
    auto name = pName.c_str();
    const char* description = nullptr;
    const void* user_data = pSetting->getUserData();
    if (pTr && user_data) {
        const std::pair<Ids, Ids>* dsc =
            reinterpret_cast<const std::pair<Ids,Ids>*>(user_data);
        if (dsc->first != Ids::_ILLEGAL_ELEMENT_) {
            name = pTr->get(dsc->first).c_str();
        }
        description = pTr->get(dsc->second).c_str();
    }

    ui::PushID(id);
    switch (set.getType()) {
    case SettingTypeEnum::BOOL:
        if (ui::Checkbox(name,
                         set.getIncomingPtr<V2::BoolValue>())) {
            set.setFromIncomingPtr<V2::BoolValue>();
        }
        break;
    case SettingTypeEnum::INT:
        if (ui::DragInt(name,
                        set.getIncomingPtr<V2::IntValue>(),
                        1, // TODO figure out proper speed
                        set.getConstraint<V2::IntValue>()._min,
                        set.getConstraint<V2::IntValue>()._max)) {
            set.setFromIncomingPtr<V2::IntValue>();
        }
        break;
    case SettingTypeEnum::FLOAT:
        if (ui::DragFloat(name,
                          set.getIncomingPtr<V2::FloatValue>(),
                          .01f, // TODO figure out proper speed
                          set.getConstraint<V2::FloatValue>()._min,
                          set.getConstraint<V2::FloatValue>()._max)) {
            set.setFromIncomingPtr<V2::FloatValue>();
        }
        break;
    case SettingTypeEnum::VECTOR3:
        if (pName.find("_color") < pName.size()) {
            if (ui::ColorEdit3(name,
                               (float*)set.getIncomingPtr<Vector3Value>())) {
                set.setFromIncomingPtr<Vector3Value>();
            }
        }
        else {
            if (ui::DragFloat3(name,
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
            StdInputText(name,
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
        if (ImGui::BeginCombo(name, curr_val.c_str(), 0)) {
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
    ui::PopID();
    ++id;

    if (description && description[0]) {
        ui::SameLine();
        imgui_render_help_marker(description);
    }

    if (is_default) {
        ui::PopStyleVar(1);
    }
}


void render_settings2_ui (const TranslationBinder *pTr, Settings2* pSettings, size_t* pLongestSettingPtr) {
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
                render_setting2_ui(pTr, set.first, set.second, i);

                if (calc_length) {
                    if (set.first.size() > * pLongestSettingPtr) {
                        *pLongestSettingPtr = set.first.size();
                    }
                }
            }
        }
    }
}

