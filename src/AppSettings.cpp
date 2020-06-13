#include "AppSettings.hpp"

#include <Urho3D/Math/Vector3.h>

AppSettings::AppSettings (Settings2& pSettings)
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
                           v3_constraint_1_to_1))) {
    _settings.load();
}
