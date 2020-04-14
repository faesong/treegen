#include <vector>
#include <deque>

#include "TreeGenSettings.hpp"
#include <UrhoBits/TreeGenerator/TreeGenerator.hpp> // TODO refactor to separate TreeConfig.hpp

using namespace V2; // TODO rename it?

struct TreeConfigCache {
    TreeConfigCache (Settings2& pSettings)
        : _cfg(pSettings) {
        _add<IntValue>(_cfg, "levels", 1, 5, 2, &_ch.num_levels);
        _add<IntValue>(_cfg, "seed", 0, 999999, 13, &_ch.seed);
        _add<FloatValue>(_cfg,
                         "branch_curvature_threshold",
                         0.f,
                         20.f,
                         5.f,
                         &_ch.branch_curvature_threshold);
        _add<FloatValue>(_cfg, "root_length", 0.1f, 200.f, 8.f, &_ch.root_length);
        _add<IntValue>(_cfg, "smoothing_iterations", 0, 3, 2, &_ch.smoothing_iterations);
        _addUnconstrained<BoolValue>(_cfg, "wireframe", false, &_ch.wireframe);
        _add<FloatValue>(_cfg, "root_radius", 0.01f, 100.f, 0.2f, &_ch.root_radius);
        _add<IntValue>(_cfg, "leaves_level", 0, UrhoBits::detail::MAX_LEAVES_LEVELS, 1,
             &_ch.leaves_level);
        _add<FloatValue>(_cfg, "leaves_size", 0.01f, 10.f, 0.25f,
             &_ch.lf_leaf_size);
        _add<FloatValue>(_cfg, "leaves_normals_outwards", 0.0f, 1.f, 0.5f,
             &_ch.leaves_normals_outwards);
        _add<IntValue>(_cfg, "leaves_per_meter", 0, 100, 15,
             &_ch.leaves_per_meter);
        _add<FloatValue>(_cfg, "leaves_offset", 0.f, .99f, 0.f,
             &_ch.leaves_offset);


        for (int i = 0; i < 6; ++i) {
            const auto is = std::to_string(i);
            _add<FloatValue>(_cfg, is + ".acacia", 0.f, 1.f, 0.f,
                 &_ch.levels[i].acacia);
            _add<IntValue>(_cfg, is + ".segments", 1, 20, 10 - i,
                 &_ch.levels[i].num_segments);
            _add<FloatValue>(_cfg, is + ".endpoint_radius_multiplier", .001f, 0.99f, .1f,
                 &_ch.levels[i].radius_multi);

            _add<FloatValue>(_cfg, is + ".children_per_meter", 0.f, 100.f, 4.f,
                 &_ch.levels[i].children_per_meter);
            _add<IntValue>(_cfg, is + ".children_min", 0, 100, 2,
                 &_ch.levels[i].children_min);
            _add<FloatValue>(_cfg, is + ".offset", 0.f, 1.f, 0.1f,
                 &_ch.levels[i].offset);
            _add<FloatValue>(_cfg, is + ".offset_shortbranch_cancel", 0.f, 1.f, 0.5f,
                 &_ch.levels[i].offset_shortbranch_cancel);

            _add<FloatValue>(_cfg, is + ".endpoint_direction_random", 0.f, 1.f, 0.3f,
                 &_ch.levels[i].endpoint_random_k);
            _add<FloatValue>(_cfg, is + ".endpoint_direction_outward", 0.f, 1.f, 0.3f,
                 &_ch.levels[i].endpoint_outward_k);
            _add<FloatValue>(_cfg, is + ".endpoint_direction_Zward", -1.f, 1.f, 0.4f,
                 &_ch.levels[i].endpoint_Zward_k);
            _add<FloatValue>(_cfg, is + ".endpoint_influence", 0.f, 10.f, 0.4f,
                 &_ch.levels[i].endpoint_influence);

            _add<FloatValue>(_cfg, is + ".growth_direction_deviation", 0.f, 1.f, 0.2f,
                 &_ch.levels[i].growth_dev);
            if (i > 0) {
                _add<FloatValue>(_cfg, is + ".length_multiplier", 0.f, 10.f, 0.3f,
                     &_ch.levels[i].length_multiplier);
                _add<FloatValue>(_cfg, is + ".length_distribution_circle", 0.f, 1.f, 0.8f,
                     &_ch.levels[i].length_distribution_circle);
                _add<FloatValue>(_cfg, is + ".length_distribution_cone", -1.f, 1.f, 0.0f,
                     &_ch.levels[i].length_distribution_cone);
                _add<FloatValue>(_cfg, is + ".length_distribution_flame", 0.f, 1.f, 0.0f,
                     &_ch.levels[i].length_distribution_flame);
                _add<FloatValue>(_cfg, is + ".length_distribution_random", 0.f, 1.f, 0.1f,
                     &_ch.levels[i].length_distribution_random);
                _add<FloatValue>(_cfg, is + ".radius_multiplier", .1f, 1.f, .35f,
                     &_ch.levels[i].branching_radius_multiplier);
                _add<FloatValue>(_cfg, is + ".direction_outward", 0.f, 1.f, 0.9f,
                     &_ch.levels[i].outward_k);
                _add<FloatValue>(_cfg, is + ".direction_Zward", -1.f, 1.f, 0.3f,
                     &_ch.levels[i].zward_k);
                _add<FloatValue>(_cfg, is + ".direction_deviation", 0.f, 1.f, 0.3f,
                     &_ch.levels[i].deviation_k);
                _add<FloatValue>(_cfg, is + ".direction_parent", -1.f, 1.f, 0.3f,
                     &_ch.levels[i].parent_k);
            }

            // we can have upto this many levels of leaves
            if (i < UrhoBits::detail::MAX_LEAVES_LEVELS) {
                const auto il = "leaves" + std::to_string(i);

                _add<FloatValue>(_cfg, il + ".direction_outward", 0.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_outward_k);
                _add<FloatValue>(_cfg, il + ".direction_Zward", -1.f, 1.f, -0.1f,
                     &_ch.leaves[i].lf_endpoint_Zward_k);
                _add<FloatValue>(_cfg, il + ".direction_random", 0.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_random_k);
                _add<FloatValue>(_cfg, il + ".direction_parent", -1.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_parent_k);
            }
        }

        _addUnconstrained<EastringValue>(_cfg,
                                         "material.leaf_texture_name",
                                         "leaf_default_3x3.dds",
                                         &leafTextureName);

        _add<IntValue>(_cfg, "material.leaves_atlas_res_x", 1, 100, 3,
                       &_ch.leaves_atlas_res_x);
        _add<IntValue>(_cfg, "material.leaves_atlas_res_y", 1, 100, 3,
                       &_ch.leaves_atlas_res_y);


        _cfg.load();
    }

    UrhoBits::TreeConfig* getTreeConfig () {
        return &_ch;
    }

    ea::string leafTextureName;

private:
    template <typename T>
    void _add (Settings2& pSettings,
          const std::string& pSettingName,
          const typename T::value_type pMin,
          const typename T::value_type pMax,
          const typename T::value_type pDefault,
          typename T::value_type* pValueToKeepUpdated) {
        auto &set = pSettings.appendSetting(
            pSettingName, T(pDefault, ArithmeticConstraint(pMin, pMax)));
        set.addUpdateHandler<T>(
            this,
            [pValueToKeepUpdated] (const typename T::value_type& pNewVal) {
                *pValueToKeepUpdated = pNewVal;
            });
        *pValueToKeepUpdated = pDefault;
    }

    template <typename T>
    void _addUnconstrained (Settings2& pSettings,
                            const std::string& pSettingName,
                            typename T::value_type pDefault,
                            typename T::value_type* pValueToKeepUpdated) {
        auto &set = pSettings.appendSetting(pSettingName, T(pDefault));
        set.addUpdateHandler<T>(
            this,
            [pValueToKeepUpdated] (const typename T::value_type& pNewVal) {
                *pValueToKeepUpdated = pNewVal;
            });
        *pValueToKeepUpdated = pDefault;
    }


    UrhoBits::TreeConfig _ch;

    Settings2& _cfg;
};

