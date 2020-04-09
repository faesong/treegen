#pragma once

#include <VcppBits/Settings/Settings.hpp>
#include <VcppBits/Settings/Setting.hpp>

struct AppSettings {
    explicit AppSettings (VcppBits::Settings& pSettings)
        : _settings (pSettings),
          tree_preset (_settings.appendSetting({"_preset", std::string() })),
          fxaa (_settings.appendSetting({"fxaa", bool(true) })),
          ssao (_settings.appendSetting({"ssao", 0.f, 10.f, 0.5f })),
          vibrance (_settings.appendSetting({"vibrance", 0.f, 10.f, 0.1f })),
          auto_exposure (_settings.appendSetting({"auto_exposure", bool(true) })){
        _settings.load();
    }

    VcppBits::Settings& _settings;

    VcppBits::Setting& tree_preset;
    VcppBits::Setting& ssao;
    VcppBits::Setting& fxaa;
    VcppBits::Setting& vibrance;
    VcppBits::Setting& auto_exposure;
};
