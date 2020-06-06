#include <vector>
#include <deque>

#include "TreeGenSettings.hpp"
#include <UrhoBits/TreeGenerator/TreeGenerator.hpp> // TODO refactor to separate TreeConfig.hpp

using namespace V2; // TODO rename it?

struct TreeConfigCache {
    TreeConfigCache (Settings2& pSettings)
        : _cfg(pSettings) {
        using namespace V2::SettingsUtils;
        createArithmetic<IntValue>(_cfg, "levels", 1, 5, 2, &_ch.num_levels);
        createArithmetic<IntValue>(_cfg, "seed", 0, 999999, 13, &_ch.seed);
        createArithmetic<FloatValue>(_cfg,
                         "branch_curvature_threshold",
                         0.f,
                         100.f,
                         5.f,
                         &_ch.branch_curvature_threshold);
        createArithmetic<FloatValue>(_cfg, "root_length", 0.1f, 200.f, 8.f, &_ch.root_length);
        createArithmetic<IntValue>(_cfg, "smoothing_iterations", 2, 20, 4, &_ch.smoothing_iterations);
        create<BoolValue>(_cfg, "wireframe", false, &_ch.wireframe);
        createArithmetic<FloatValue>(_cfg, "root_radius", 0.01f, 100.f, 0.2f, &_ch.root_radius);
        createArithmetic<IntValue>(_cfg, "leaves_level", 0, UrhoBits::detail::MAX_LEAVES_LEVELS, 1,
             &_ch.leaves_level);
        createArithmetic<FloatValue>(_cfg, "leaves_size", 0.01f, 10.f, 0.25f,
             &_ch.lf_leaf_size);
        createArithmetic<FloatValue>(_cfg, "leaves_normals_outwards", 0.0f, 1.f, 0.5f,
             &_ch.leaves_normals_outwards);
        createArithmetic<IntValue>(_cfg, "leaves_per_meter", 0, 100, 15,
             &_ch.leaves_per_meter);
        createArithmetic<FloatValue>(_cfg, "leaves_offset", 0.f, .99f, 0.f,
             &_ch.leaves_offset);


        for (int i = 0; i < 6; ++i) {
            const auto is = std::to_string(i);
            createArithmetic<FloatValue>(_cfg, is + ".acacia", 0.f, 1.f, 0.f,
                 &_ch.levels[i].acacia);
            createArithmetic<IntValue>(_cfg, is + ".segments", 1, 20, 10 - i,
                 &_ch.levels[i].num_segments);
            createArithmetic<FloatValue>(_cfg, is + ".endpoint_radius_multiplier", .001f, 0.99f, .1f,
                 &_ch.levels[i].radius_multi);

            createArithmetic<FloatValue>(_cfg, is + ".children_per_meter", 0.f, 100.f, 4.f,
                 &_ch.levels[i].children_per_meter);
            createArithmetic<IntValue>(_cfg, is + ".children_min", 0, 100, 2,
                 &_ch.levels[i].children_min);
            createArithmetic<FloatValue>(_cfg, is + ".offset", 0.f, 1.f, 0.1f,
                 &_ch.levels[i].offset);
            createArithmetic<FloatValue>(_cfg, is + ".offset_shortbranch_cancel", 0.f, 1.f, 0.5f,
                 &_ch.levels[i].offset_shortbranch_cancel);

            createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_random", 0.f, 1.f, 0.3f,
                 &_ch.levels[i].endpoint_random_k);
            createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_outward", 0.f, 1.f, 0.3f,
                 &_ch.levels[i].endpoint_outward_k);
            createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_Zward", -1.f, 1.f, 0.4f,
                 &_ch.levels[i].endpoint_Zward_k);
            createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_crown_outward", 0.f, 1.f, 0.2f,
                             &_ch.levels[i].endpoint_crown_outward_k);
            createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_parent", 0.f, 1.f, 0.2f,
                             &_ch.levels[i].endpoint_parent_k);

            createArithmetic<FloatValue>(_cfg, is + ".endpoint_influence", 0.f, 10.f, 0.4f,
                 &_ch.levels[i].endpoint_influence);

            createArithmetic<FloatValue>(_cfg, is + ".growth_direction_deviation", 0.f, 1.f, 0.2f,
                 &_ch.levels[i].growth_dev);
            if (i > 0) {
                createArithmetic<FloatValue>(_cfg, is + ".length_multiplier", 0.f, 10.f, 0.3f,
                     &_ch.levels[i].length_multiplier);
                createArithmetic<FloatValue>(_cfg, is + ".length_distribution_circle", 0.f, 1.f, 0.8f,
                     &_ch.levels[i].length_distribution_circle);
                createArithmetic<FloatValue>(_cfg, is + ".length_distribution_cone", -1.f, 1.f, 0.0f,
                     &_ch.levels[i].length_distribution_cone);
                createArithmetic<FloatValue>(_cfg, is + ".length_distribution_flame", 0.f, 1.f, 0.0f,
                     &_ch.levels[i].length_distribution_flame);
                createArithmetic<FloatValue>(_cfg, is + ".length_distribution_random", 0.f, 1.f, 0.1f,
                     &_ch.levels[i].length_distribution_random);
                createArithmetic<FloatValue>(_cfg, is + ".radius_multiplier", .1f, 1.f, .35f,
                     &_ch.levels[i].branching_radius_multiplier);
                createArithmetic<FloatValue>(_cfg, is + ".direction_outward", 0.f, 1.f, 0.9f,
                     &_ch.levels[i].outward_k);
                createArithmetic<FloatValue>(_cfg, is + ".direction_Zward", -1.f, 1.f, 0.3f,
                     &_ch.levels[i].zward_k);
                createArithmetic<FloatValue>(_cfg, is + ".direction_deviation", 0.f, 1.f, 0.3f,
                     &_ch.levels[i].deviation_k);
                createArithmetic<FloatValue>(_cfg, is + ".direction_parent", -1.f, 1.f, 0.3f,
                     &_ch.levels[i].parent_k);
                createArithmetic<FloatValue>(_cfg, is + ".direction_crown_outward", 0.f, 1.f, 0.1f,
                                 &_ch.levels[i].crown_outward_k);
                createArithmetic<FloatValue>(_cfg, is + ".crown_pos_offset", -.5f, .5f, -.2f,
                                 &_ch.levels[i].crownpos_offset);
            }

            // we can have upto this many levels of leaves
            if (i < UrhoBits::detail::MAX_LEAVES_LEVELS) {
                const auto il = "leaves" + std::to_string(i);

                createArithmetic<FloatValue>(_cfg, il + ".direction_outward", 0.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_outward_k);
                createArithmetic<FloatValue>(_cfg, il + ".direction_Zward", -1.f, 1.f, -0.1f,
                     &_ch.leaves[i].lf_endpoint_Zward_k);
                createArithmetic<FloatValue>(_cfg, il + ".direction_random", 0.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_random_k);
                createArithmetic<FloatValue>(_cfg, il + ".direction_parent", -1.f, 1.f, 0.5f,
                     &_ch.leaves[i].lf_endpoint_parent_k);

                createArithmetic<FloatValue>(_cfg, il + ".normal_parent", 0.001f, 1.f, 1.f,
                                 &_ch.leaves[i].lf_normal_parent_k);
                createArithmetic<FloatValue>(_cfg, il + ".normal_outward", 0.001f, 1.f, 0.001f,
                                 &_ch.leaves[i].lf_normal_outward_k);
            }
        }

        create<V2::StringValue>(_cfg,
                                "material.leaf_texture_name",
                                "leaf_default.png",
                                &leafTextureName);

        createArithmetic<IntValue>(_cfg, "material.leaves_atlas_res_x", 1, 100, 3,
                       &_ch.leaves_atlas_res_x);
        createArithmetic<IntValue>(_cfg, "material.leaves_atlas_res_y", 1, 100, 3,
                       &_ch.leaves_atlas_res_y);

        createEnum<V2::EnumStringValue>(
            _cfg,
            "material.leaves_shape",
            { "diamond", "square" },
            "diamond",
            &leaves_shape,
            (int*) &_ch.leaves_shape);

        _cfg.load();
    }

    UrhoBits::TreeConfig* getTreeConfig () {
        return &_ch;
    }

    std::string leafTextureName;

    std::string leaves_shape;

private:
    UrhoBits::TreeConfig _ch;

    Settings2& _cfg;
};

