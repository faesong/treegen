#pragma once

#include "TreeGenSettings.hpp"

struct AppSettings {
    inline static const auto v3_constraint_0_to_1 =
        V2::GenericArithmeticConstraint(Urho3D::Vector3(0.f, 0.f, 0.f),
                                        Urho3D::Vector3(1.f, 1.f, 1.f));
    inline static const auto v3_constraint_1_to_1 =
        V2::GenericArithmeticConstraint(Urho3D::Vector3(-1.f, -1.f, -1.f),
            Urho3D::Vector3(1.f, 1.f, 1.f));

    explicit AppSettings(Settings2& pSettings);

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
