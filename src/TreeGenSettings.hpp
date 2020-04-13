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
