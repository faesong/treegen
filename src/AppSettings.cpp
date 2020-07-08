// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "AppSettings.hpp"

#include <Urho3D/Math/Vector3.h>

#include <VcppBits/Translation/Translation.hpp>

#include "TranslationUtils.hpp"


using namespace VcppBits::Translation;
using namespace V2::SettingsUtils;

AppSettings::AppSettings (Settings2& pSettings,
                          Translation& pTranslation)
    : _tr (pTranslation),
      _settings(pSettings),
      // TODO7: create new type of setting constraint: the one which uses a
      // callback to provide the list of possible options on-the-fly
      language(createEnum<V2::EnumStringValue>(
                   _settings,
                   "language",
                   _tr.getLanguages(),
                   "English",
                   &language_,
                   nullptr)
               .setUserData(createDescription(_dsc,
                                              Ids::LANGUAGE_SETTING,
                                              Ids::_ILLEGAL_ELEMENT_))),
      tree_preset(
          _settings
          .appendSetting("_preset", V2::StringValue(""))
          .setUserData(createDescription(_dsc,
                                         Ids::TREE_PRESET_SETTING,
                                         Ids::_ILLEGAL_ELEMENT_))),
      ssao(
          _settings
          .appendSetting(
              "ssao",
              V2::FloatValue(.5f, V2::ArithmeticConstraint(0.f, 10.f)))
          .setUserData(createDescription(_dsc,
                                         Ids::SSAO_SETTING,
                                         Ids::SSAO_SETTING_DESCRIPTION))),
      fxaa(
          _settings
          .appendSetting("fxaa", V2::BoolValue(true))
          .setUserData(createDescription(_dsc,
                                         Ids::FXAA_SETTING,
                                         Ids::FXAA_SETTING_DESCRIPTION))),
      vibrance(
          _settings
          .appendSetting(
              "vibrance",
              V2::FloatValue(.5f, V2::ArithmeticConstraint(0.f, 10.f)))
          .setUserData(createDescription(_dsc,
                                         Ids::VIBRANCE_SETTING,
                                         Ids::VIBRANCE_SETTING_DESCRIPTION))),
      shadow_intensity(
          _settings
          .appendSetting(
              "shadow_intensity",
              V2::FloatValue(.1f, V2::ArithmeticConstraint(0.f, 1.f)))
          .setUserData(createDescription(_dsc,
                                         Ids::SHADOW_INTENSITY_SETTING,
                                         Ids::_ILLEGAL_ELEMENT_))),
      auto_exposure(
          _settings
          .appendSetting("auto_exposure", V2::BoolValue(true))
          .setUserData(createDescription(_dsc,
                                         Ids::AUTO_EXPOSURE_SETTING,
                                         Ids::AUTO_EXPOSURE_SETTING_DESCRIPTION))),
      ambient(
          _settings
          .appendSetting(
              "ambient_color",
              Vector3Value(Urho3D::Vector3(0.5, 0.5, 0.5),
                           v3_constraint_0_to_1))
          .setUserData(createDescription(_dsc,
                                         Ids::AMBIENT_COLOR_SETTING,
                                         Ids::AMBIENT_COLOR_SETTING_DESCRIPTION))),
      fog_color(
          _settings
          .appendSetting(
              "fog_color",
              Vector3Value(Urho3D::Vector3(0.5, 0.5, 0.5),
                           v3_constraint_0_to_1))
          .setUserData(createDescription(_dsc,
                                         Ids::FOG_COLOR_SETTING,
                                         Ids::FOG_COLOR_SETTING_DESCRIPTION))),
      light_direction(
          _settings
          .appendSetting(
              "light_direction",
              Vector3Value(Urho3D::Vector3(0.f, -1.f, -1.f),
                           v3_constraint_1_to_1))
          .setUserData(createDescription(_dsc,
                                         Ids::LIGHT_DIRECTION_SETTING,
                                         Ids::_ILLEGAL_ELEMENT_))) {
    _settings.load();
}

