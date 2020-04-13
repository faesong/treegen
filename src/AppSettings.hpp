#pragma once

#include <VcppBits/Settings2/Settings2.hpp>

inline std::string vector3_to_string (const Urho3D::Vector3 pVector3) {
    std::stringstream ss;
    ss << std::fixed;
    ss << "Vector3(" << pVector3.x_ << " "
       << pVector3.y_ << " "
       << pVector3.z_ << ")";
    return ss.str();
}

inline Urho3D::Vector3 vector3_from_string (const std::string &pStr) {
    using VcppBits::StringUtils::fromString;
    if (pStr.find("Vector3(") == 0
        && pStr[pStr.size() - 1] == ')') {
        Urho3D::Vector3 ret;
        std::stringstream ss(pStr.substr(8, pStr.size() - 1));
        ss >> ret.x_ >> ret.y_ >> ret.z_;

        return ret;
    }

    throw std::runtime_error("couldn't parse Vector3 from: " + pStr);
}

namespace Urho3D {
    bool operator>= (const Vector3 &pLeft, const Vector3 pRight) {
        return pLeft.x_ >= pRight.x_
            && pLeft.y_ >= pRight.y_
            && pLeft.z_ >= pRight.z_;
    }
    bool operator<= (const Vector3 &pLeft, const Vector3 pRight) {
        return pRight >= pLeft;
    }

} // namespace Urho3D

using Vector3Value =
          V2::SettingValue<Urho3D::Vector3,
                           V2::GenericArithmeticConstraint,
                           vector3_to_string,
                           vector3_from_string>;

enum class SettingTypeEnum : std::size_t { BOOL,
                                           INT,
                                           FLOAT,
                                           ENUM_INT,
                                           ENUM_FLOAT,
                                           STRING,
                                           ENUM_STRING,
                                           VECTOR3 };

using Setting2 = V2::SettingImpl<SettingTypeEnum,
                                 V2::BoolValue,
                                 V2::IntValue,
                                 V2::FloatValue,
                                 V2::EnumIntValue,
                                 V2::EnumFloatValue,
                                 V2::StringValue,
                                 V2::EnumStringValue,
                                 Vector3Value>;

using Settings2 = V2::SettingsImpl<Setting2>;

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
          fxaa(
              _settings
              .appendSetting("fxaa", V2::BoolValue(true))),
          ssao(
              _settings
              .appendSetting(
                  "ssao",
                  V2::FloatValue(.5f, V2::ArithmeticConstraint(0.f, 10.f)))),
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
                               v3_constraint_1_to_1))),
          auto_exposure(
              _settings
              .appendSetting("auto_exposure", V2::BoolValue(true))) {
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
