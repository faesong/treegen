#include <vector>
#include <deque>

#include <VcppBits/Settings/Settings.hpp>
#include <UrhoBits/TreeGenerator/TreeGenerator.hpp> // TODO refactor to separate TreeConfig.hpp



struct TreeConfigCache {
    TreeConfigCache (VcppBits::Settings& pSettings)
        : _cfg(pSettings) {
        using VcppBits::Setting;
       _add(_cfg.appendSetting(Setting("levels", 1, 5, 2)),
            &_ch.num_levels);
       _add(_cfg.appendSetting(Setting("seed", 0, 999999, 13)),
            &_ch.seed);
       _add(_cfg.appendSetting(Setting("branch_curvature_threshold", 0.f, 20.f, 5.f)),
            &_ch.branch_curvature_threshold);
       _add(_cfg.appendSetting(Setting("root_length", 0.1f, 200.f, 8.f)),
            &_ch.root_length);
       _add(_cfg.appendSetting(Setting("smoothing_iterations", 0, 3, 2)),
            &_ch.smoothing_iterations);
       _add(_cfg.appendSetting(Setting("wireframe", static_cast<bool>(false))),
            &_ch.wireframe);
       _add(_cfg.appendSetting(Setting("root_radius", 0.01f, 100.f, 0.2f)),
            &_ch.root_radius);
       _add(_cfg.appendSetting(Setting("leaves_level", 0, UrhoBits::detail::MAX_LEAVES_LEVELS, 1)),
            &_ch.leaves_level);
       _add(_cfg.appendSetting(Setting("leaves_size", 0.01f, 10.f, 0.25f)),
            &_ch.lf_leaf_size);
       _add(_cfg.appendSetting(Setting("leaves_normals_outwards", 0.0f, 1.f, 0.5f)),
            &_ch.leaves_normals_outwards);
       _add(_cfg.appendSetting(Setting("leaves_per_meter", 0, 100, 15)),
            &_ch.leaves_per_meter);
       _add(_cfg.appendSetting(Setting("leaves_offset", 0.f, .99f, 0.f)),
            &_ch.leaves_offset);


       for (int i = 0; i < 6; ++i) {
           const auto is = std::to_string(i);
           _add(_cfg.appendSetting(Setting(is + ".acacia", 0.f, 1.f, 0.f)),
                &_ch.levels[i].acacia);
           _add(_cfg.appendSetting(Setting(is + ".segments", 1, 20, 10 - i)),
                &_ch.levels[i].num_segments);
           _add(_cfg.appendSetting(Setting(is + ".endpoint_radius_multiplier", .001f, 0.99f, .1f)),
                &_ch.levels[i].radius_multi);

           _add(_cfg.appendSetting(Setting(is + ".children_per_meter", 0.f, 100.f, 4.f)),
                &_ch.levels[i].children_per_meter);
           _add(_cfg.appendSetting(Setting(is + ".children_min", 0, 100, 2)),
                &_ch.levels[i].children_min);
           _add(_cfg.appendSetting(Setting(is + ".offset", 0.f, 1.f, 0.1f)),
                &_ch.levels[i].offset);
           _add(_cfg.appendSetting(Setting(is + ".offset_shortbranch_cancel", 0.f, 1.f, 0.5f)),
                &_ch.levels[i].offset_shortbranch_cancel);

           _add(_cfg.appendSetting(Setting(is + ".endpoint_direction_random", 0.f, 1.f, 0.3f)),
                &_ch.levels[i].endpoint_random_k);
           _add(_cfg.appendSetting(Setting(is + ".endpoint_direction_outward", 0.f, 1.f, 0.3f)),
                &_ch.levels[i].endpoint_outward_k);
           _add(_cfg.appendSetting(Setting(is + ".endpoint_direction_Zward", -1.f, 1.f, 0.4f)),
                &_ch.levels[i].endpoint_Zward_k);
           _add(_cfg.appendSetting(Setting(is + ".endpoint_influence", 0.f, 10.f, 0.4f)),
                &_ch.levels[i].endpoint_influence);

           _add(_cfg.appendSetting(Setting(is + ".growth_direction_deviation", 0.f, 1.f, 0.2f)),
                &_ch.levels[i].growth_dev);
           if (i > 0) {
               _add(_cfg.appendSetting(Setting(is + ".length_multiplier", 0.f, 10.f, 0.3f)),
                    &_ch.levels[i].length_multiplier);
               _add(_cfg.appendSetting(Setting(is + ".length_distribution_circle", 0.f, 1.f, 0.8f)),
                    &_ch.levels[i].circle_function_k);
               _add(_cfg.appendSetting(Setting(is + ".length_distribution_cone", -1.f, 1.f, 0.0f)),
                    &_ch.levels[i].conical_cfg);
               _add(_cfg.appendSetting(Setting(is + ".length_distribution_flame", 0.f, 1.f, 0.0f)),
                    &_ch.levels[i].flame_k);
               _add(_cfg.appendSetting(Setting(is + ".radius_multiplier", .1f, 1.f, .35f)),
                    &_ch.levels[i].branching_radius_multiplier);
               _add(_cfg.appendSetting(Setting(is + ".direction_outward", 0.f, 1.f, 0.9f)),
                    &_ch.levels[i].outward_k);
               _add(_cfg.appendSetting(Setting(is + ".direction_Zward", -1.f, 1.f, 0.3f)),
                    &_ch.levels[i].zward_k);
               _add(_cfg.appendSetting(Setting(is + ".direction_deviation", 0.f, 1.f, 0.3f)),
                    &_ch.levels[i].deviation_k);
               _add(_cfg.appendSetting(Setting(is + ".direction_parent", -1.f, 1.f, 0.3f)),
                    &_ch.levels[i].parent_k);
           }

           // we can have upto this many levels of leaves
           if (i < UrhoBits::detail::MAX_LEAVES_LEVELS) {
               const auto il = "leaves" + std::to_string(i);

               _add(_cfg.appendSetting(Setting(il + ".direction_outward", 0.f, 1.f, 0.5f)),
                    &_ch.leaves[i].lf_endpoint_outward_k);
               _add(_cfg.appendSetting(Setting(il + ".direction_Zward", -1.f, 1.f, -0.1f)),
                    &_ch.leaves[i].lf_endpoint_Zward_k);
               _add(_cfg.appendSetting(Setting(il + ".direction_random", 0.f, 1.f, 0.5f)),
                    &_ch.leaves[i].lf_endpoint_random_k);
               _add(_cfg.appendSetting(Setting(il + ".direction_parent", -1.f, 1.f, 0.5f)),
                    &_ch.leaves[i].lf_endpoint_parent_k);
           }
       }

        _cfg.load();
    }

    UrhoBits::TreeConfig* getTreeConfig () {
        return &_ch;
    }

private:
    template <typename T>
    void _add (VcppBits::Setting& pSetting, T* pValuePtr) {
        _updaters.emplace_back(pSetting, pValuePtr);
    }

    UrhoBits::TreeConfig _ch;

    std::vector<VcppBits::SettingUpdater> _settings; // TODO use array
    VcppBits::Settings& _cfg;
    std::deque<VcppBits::SettingUpdater> _updaters;
};

