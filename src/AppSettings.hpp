#pragma once

#include "TreeGenSettings.hpp"

struct AppSettings {
    inline static const auto v3_constraint_0_to_1 =
        V2::GenericArithmeticConstraint(Urho3D::Vector3(0.f, 0.f, 0.f),
                                        Urho3D::Vector3(1.f, 1.f, 1.f));
    inline static const auto v3_constraint_1_to_1 =
        V2::GenericArithmeticConstraint(Urho3D::Vector3(-1.f, -1.f, -1.f),
            Urho3D::Vector3(1.f, 1.f, 1.f));
    explicit AppSettings (Settings2& pSettings)
        : _settings(pSettings),
          tree_preset(
              _settings
              .appendSetting("_preset", V2::StringValue(""))),
          ssao(
              _settings
              .appendSetting(
                  "ssao",
                  V2::FloatValue(.5f, V2::ArithmeticConstraint(0.f, 10.f)))),
          fxaa(
              _settings
              .appendSetting("fxaa", V2::BoolValue(true))),
          vibrance(
              _settings
              .appendSetting(
                  "vibrance",
                  V2::FloatValue(.5f, V2::ArithmeticConstraint(0.f, 10.f)))),
          shadow_intensity(
              _settings
              .appendSetting(
                  "shadow_intensity",
                  V2::FloatValue(.1f, V2::ArithmeticConstraint(0.f, 1.f)))),
          auto_exposure(
              _settings
              .appendSetting("auto_exposure", V2::BoolValue(true))),
          ambient(
              _settings
              .appendSetting(
                  "ambient_color",
                  Vector3Value(Urho3D::Vector3(0.5, 0.5, 0.5),
                               v3_constraint_0_to_1))),
          fog_color(
              _settings
              .appendSetting(
                  "fog_color",
                  Vector3Value(Urho3D::Vector3(0.5, 0.5, 0.5),
                               v3_constraint_0_to_1))),
          light_direction(
              _settings
              .appendSetting(
                  "light_direction",
                  Vector3Value(Urho3D::Vector3(0.5, -0.5, 0.5),
                               v3_constraint_1_to_1)))
 {
        _settings.load();
    }

    Settings2& _settings;

    Setting2& tree_preset;
    Setting2& ssao;
    Setting2& fxaa;
    Setting2& vibrance;
    Setting2& shadow_intensity;
    Setting2& auto_exposure;
    Setting2& ambient;
    Setting2& fog_color;
    Setting2& light_direction;
};
