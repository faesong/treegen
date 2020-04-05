#pragma once

#include <VcppBits/Settings/Settings.hpp>
#include <VcppBits/Settings/Setting.hpp>

struct AppSettings {
    explicit AppSettings (VcppBits::Settings& pSettings)
        : _settings (pSettings),
          tree_preset (_settings.appendSetting({"_preset", std::string() })){
        _settings.load();
    }

    VcppBits::Settings& _settings;

    VcppBits::Setting& tree_preset;
};
