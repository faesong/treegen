#pragma once

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
