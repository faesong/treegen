#include "TreeConfigCache.hpp"

using namespace V2; // TODO rename/remove

#include <VcppBits/Translation/Translation.hpp>


#include "TranslationUtils.hpp"


using Ids = VcppBits::Translation::Ids;


TreeConfigCache::TreeConfigCache (Settings2& pSettings)
    : _cfg(pSettings) {
    using namespace V2::SettingsUtils;
    static std::deque<std::pair<Ids, Ids>> dsc;


    createArithmetic<IntValue>(_cfg, "levels", 1, 5, 2, &_ch.num_levels)
        .setUserData(createDescription(dsc,
                                       Ids::LEVELS_TSETTING,
                                       Ids::LEVELS_TSETTING_DESCRIPTION));
    createArithmetic<IntValue>(_cfg, "seed", 0, 999999, 13, &_ch.seed)
        .setUserData(createDescription(dsc, Ids::SEED_TSETTING));
    createArithmetic<FloatValue>(_cfg,
                                 "branch_curvature_threshold",
                                 0.f,
                                 100.f,
                                 5.f,
                                 &_ch.branch_curvature_threshold)
        .setUserData(createDescription(dsc,
                                       Ids::CURVATURE_THRESHOLD_TSETTING,
                                       Ids::CURVATURE_THRESHOLD_TSETTING_DESCRIPTION));
    createArithmetic<FloatValue>(_cfg, "root_length", 0.1f, 200.f, 8.f, &_ch.root_length)
        .setUserData(createDescription(dsc, Ids::ROOT_LENGTH_TSETTING));
    createArithmetic<IntValue>(_cfg, "smoothing_iterations", 2, 100, 4, &_ch.smoothing_iterations)
        .setUserData(createDescription(dsc,
                                       Ids::SMOOTHING_ITERATIONS_TSETTING,
                                       Ids::SMOOTHING_ITERATIONS_TSETTING_DESCRIPTION));
    create<BoolValue>(_cfg, "wireframe", false, &_ch.wireframe)
        .setUserData(createDescription(dsc, Ids::WIREFRAME_TSETTING));
    createArithmetic<FloatValue>(_cfg, "root_radius", 0.01f, 100.f, .1f, &_ch.root_radius)
        .setUserData(createDescription(dsc, Ids::ROOT_RADIUS_TSETTING));
    createArithmetic<IntValue>(_cfg, "leaves_level", 0, UrhoBits::detail::MAX_LEAVES_LEVELS, 1,
                               &_ch.leaves_level)
        .setUserData(createDescription(dsc,
                                       Ids::LEAVES_LEVEL_TSETTING,
                                       Ids::LEAVES_LEVEL_TSETTING_DESCRIPTION));
    createArithmetic<FloatValue>(_cfg, "leaves_size", 0.01f, 10.f, 0.25f,
                                 &_ch.lf_leaf_size)
        .setUserData(createDescription(dsc, Ids::LEAVES_SIZE_TSETTING));
    createArithmetic<FloatValue>(_cfg, "leaves_normals_outwards", 0.0f, 1.f, 0.5f,
                                 &_ch.leaves_normals_outwards)
        .setUserData(createDescription(dsc,
                                       Ids::LEAVES_NORMALS_OUTWARDS_TSETTING,
                                       Ids::LEAVES_NORMALS_OUTWARDS_TSETTING_DESCRIPTION));
    createArithmetic<IntValue>(_cfg, "leaves_per_meter", 0, 100, 45,
                               &_ch.leaves_per_meter)
        .setUserData(createDescription(dsc,
                                       Ids::LEAVES_PER_METER_TSETTING,
                                       Ids::LEAVES_PER_METER_TSETTING_DESCRIPTION));
    createArithmetic<FloatValue>(_cfg, "leaves_offset", 0.f, .99f, 0.f,
                                 &_ch.leaves_offset)
        .setUserData(createDescription(dsc,
                                       Ids::LEAVES_OFFSET_TSETTING,
                                       Ids::LEAVES_OFFSET_TSETTING_DESCRIPTION));


    for (size_t i = 0; i < 6; ++i) {
        const auto is = std::to_string(i);
        createArithmetic<FloatValue>(_cfg, is + ".acacia", 0.f, 1.f, 0.f,
                                     &_ch.levels[i].acacia)
            .setUserData(createDescription(dsc,
                                           Ids::ACACIA_TSETTING,
                                           Ids::ACACIA_TSETTING_DESCRIPTION));
        createArithmetic<IntValue>(_cfg, is + ".segments", 1, 20, 10 - i,
                                   &_ch.levels[i].num_segments)
            .setUserData(createDescription(dsc,
                                           Ids::SEGMENTS_TSETTING,
                                           Ids::SEGMENTS_TSETTING_DESCRIPTION));
        createArithmetic<FloatValue>(_cfg, is + ".endpoint_radius_multiplier", .001f, 0.99f, .1f,
                                     &_ch.levels[i].radius_multi)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_RADIUS_MULTIPLIER_TSETTING));
        createArithmetic<FloatValue>(_cfg, is + ".children_per_meter", 0.f, 100.f, 8.f,
                                     &_ch.levels[i].children_per_meter)
            .setUserData(createDescription(dsc,
                                           Ids::CHILDREN_PER_METER_TSETTING));
        createArithmetic<IntValue>(_cfg, is + ".children_min", 0, 100, 2,
                                   &_ch.levels[i].children_min)
            .setUserData(createDescription(dsc,
                                           Ids::CHILDREN_MIN_TSETTING));
        createArithmetic<FloatValue>(_cfg, is + ".offset", 0.f, 3.f, 0.3f,
                                     &_ch.levels[i].offset)
            .setUserData(createDescription(dsc,
                                           Ids::OFFSET_TSETTING));
        createArithmetic<FloatValue>(_cfg, is + ".offset_shortbranch_cancel", 0.f, 1.f, 0.85f,
                                     &_ch.levels[i].offset_shortbranch_cancel)
            .setUserData(createDescription(dsc,
                                           Ids::OFFSET_SHORTBRANCH_CANCEL_TSETTING,
                                           Ids::OFFSET_SHORTBRANCH_CANCEL_TSETTING_DESCRIPTION));

        createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_random", 0.f, 1.f, 0.3f,
                                     &_ch.levels[i].endpoint_random_k)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_DIRECTION_RANDOM_TSETTING));
        createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_outward", 0.f, 1.f, 0.3f,
                                     &_ch.levels[i].endpoint_outward_k)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_DIRECTION_OUTWARD_TSETTING,
                                           Ids::ENDPOINT_DIRECTION_OUTWARD_TSETTING_DESCRIPTION));
        createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_Zward", -1.f, 1.f, 0.4f,
                                     &_ch.levels[i].endpoint_Zward_k)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_DIRECTION_ZWARD_TSETTING));
        createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_crown_outward", 0.f, 1.f, 0.2f,
                                     &_ch.levels[i].endpoint_crown_outward_k)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_DIRECTION_CROWN_OUTWARD_TSETTING,
                                           Ids::ENDPOINT_DIRECTION_CROWN_OUTWARD_TSETTING_DESCRIPTION));
        createArithmetic<FloatValue>(_cfg, is + ".endpoint_direction_parent", 0.f, 1.f, 0.2f,
                                     &_ch.levels[i].endpoint_parent_k)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_DIRECTION_PARENT_TSETTING,
                                           Ids::ENDPOINT_DIRECTION_PARENT_TSETTING_DESCRIPTION));

        createArithmetic<FloatValue>(_cfg, is + ".endpoint_influence", 0.f, 10.f, 0.4f,
                                     &_ch.levels[i].endpoint_influence)
            .setUserData(createDescription(dsc,
                                           Ids::ENDPOINT_INFLUENCE_TSETTING,
                                           Ids::ENDPOINT_INFLUENCE_TSETTING_DESCRIPTION));

        createArithmetic<FloatValue>(_cfg, is + ".growth_direction_deviation", 0.f, 1.f, 0.2f,
                                     &_ch.levels[i].growth_dev)
            .setUserData(createDescription(dsc,
                                           Ids::GROWTH_DIRECTION_DEVIATION_TSETTING,
                                           Ids::GROWTH_DIRECTION_DEVIATION_TSETTING_DESCRIPTION));
        if (i > 0) {
            createArithmetic<FloatValue>(_cfg, is + ".length_multiplier", 0.f, 10.f, 0.15f + float(i) * .12f,
                                         &_ch.levels[i].length_multiplier)
                .setUserData(createDescription(dsc,
                                               Ids::LENGTH_MULTIPLIER_TSETTING,
                                               Ids::LENGTH_MULTIPLIER_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".length_distribution_circle", 0.f, 1.f, 0.8f,
                                         &_ch.levels[i].length_distribution_circle)
                .setUserData(createDescription(dsc,
                                               Ids::LENGTH_DISTRIBUTION_CIRCLE_TSETTING,
                                               Ids::LENGTH_DISTRUBUTION_CIRCLE_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".length_distribution_cone", -1.f, 1.f, 0.0f,
                                         &_ch.levels[i].length_distribution_cone)
                .setUserData(createDescription(dsc,
                                               Ids::LENGTH_DISTRIBUTION_CONE_TSETTING,
                                               Ids::LENGTH_DISTRIBUTION_CONE_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".length_distribution_flame", 0.f, 1.f, 0.0f,
                                         &_ch.levels[i].length_distribution_flame)
                .setUserData(createDescription(dsc,
                                               Ids::LENGTH_DISTRIBUTION_FLAME_TSETTING,
                                               Ids::LENGTH_DISTRIBUTION_FLAME_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".length_distribution_random", 0.f, 1.f, 0.1f,
                                         &_ch.levels[i].length_distribution_random)
                .setUserData(createDescription(dsc,
                                               Ids::LENGTH_DISTRIBUTION_RANDOM_TSETTING,
                                               Ids::LENGTH_DISTRIBUTION_RANDOM_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".radius_multiplier", .1f, 1.f, .35f,
                                         &_ch.levels[i].branching_radius_multiplier)
                .setUserData(createDescription(dsc,
                                               Ids::RADIUS_MULTIPLIER_TSETTING,
                                               Ids::RADIUS_MULTIPLIER_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".direction_outward", 0.f, 1.f, 0.9f,
                                         &_ch.levels[i].outward_k)
                .setUserData(createDescription(dsc,
                                               Ids::DIRECTION_OUTWARD_TSETTING,
                                               Ids::DIRECTION_OUTWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".direction_Zward", -1.f, 1.f, 0.3f,
                                         &_ch.levels[i].zward_k)
                .setUserData(createDescription(dsc,
                                               Ids::DIRECTION_UPWARD_TSETTING,
                                               Ids::DIRECTION_UPWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".direction_deviation", 0.f, 1.f, 0.3f,
                                         &_ch.levels[i].deviation_k)
                .setUserData(createDescription(dsc,
                                               Ids::DIRECTION_DEVIATION_TSETTING,
                                               Ids::DIRECTION_DEVIATION_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".direction_parent", -1.f, 1.f, 0.3f,
                                         &_ch.levels[i].parent_k)
                .setUserData(createDescription(dsc,
                                               Ids::DIRECTION_PARENT_TSETTING,
                                               Ids::DIRECTION_PARENT_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".direction_crown_outward", 0.f, 1.f, 0.1f,
                                         &_ch.levels[i].crown_outward_k)
                .setUserData(createDescription(dsc,
                                               Ids::DIRECTION_CROWN_OUTWARD_TSETTING,
                                               Ids::DIRECTION_CROWN_OUTWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, is + ".crown_pos_offset", -.5f, .5f, -.2f,
                                         &_ch.levels[i].crownpos_offset)
                .setUserData(createDescription(dsc,
                                               Ids::CROWN_POS_OFFSET_TSETTING,
                                               Ids::CROWN_POS_OFFSET_TSETTING_DESCRIPTION));
        }

        // we can have upto this many levels of leaves
        if (i < UrhoBits::detail::MAX_LEAVES_LEVELS) {
            const auto il = "leaves" + std::to_string(i);

            createArithmetic<FloatValue>(_cfg, il + ".direction_outward", 0.f, 1.f, 0.5f,
                                         &_ch.leaves[i].lf_endpoint_outward_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_DIRECTION_OUTWARD_TSETTING,
                                               Ids::LEAF_DIRECTION_OUTWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, il + ".direction_Zward", -1.f, 1.f, -0.1f,
                                         &_ch.leaves[i].lf_endpoint_Zward_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_DIRECTION_UPWARD_TSETTING,
                                               Ids::LEAF_DIRECTION_UPWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, il + ".direction_random", 0.f, 1.f, 0.5f,
                                         &_ch.leaves[i].lf_endpoint_random_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_DIRECTION_RANDOM_TSETTING,
                                               Ids::LEAF_DIRECTION_RANDOM_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, il + ".direction_parent", -1.f, 1.f, 0.5f,
                                         &_ch.leaves[i].lf_endpoint_parent_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_DIRECTION_PARENT_TSETTING,
                                               Ids::LEAF_DIRECTION_PARENT_TSETTING_DESCRIPTION));

            createArithmetic<FloatValue>(_cfg, il + ".normal_parent", 0.001f, 1.f, 1.f,
                                         &_ch.leaves[i].lf_normal_parent_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_NORMAL_PARENT_TSETTING,
                                               Ids::LEAF_NORMAL_PARENT_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, il + ".normal_outward", 0.001f, 1.f, 0.001f,
                                         &_ch.leaves[i].lf_normal_outward_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_NORMAL_OUTWARD_TSETTING,
                                               Ids::LEAF_NORMAL_OUTWARD_TSETTING_DESCRIPTION));
            createArithmetic<FloatValue>(_cfg, il + ".normal_upward", 0.001f, 1.f, 0.001f,
                                         &_ch.leaves[i].lf_normal_upward_k)
                .setUserData(createDescription(dsc,
                                               Ids::LEAF_NORMAL_UPWARD_TSETTING,
                                               Ids::LEAF_NORMAL_UPWARD_TSETTING_DESCRIPTION));

        }
    }

    create<V2::StringValue>(_cfg,
                            "material.leaf_texture_name",
                            "birch.png",
                            &leafTextureName)
        .setUserData(createDescription(dsc,
                                       Ids::LEAF_TEXTURE_NAME_TSETTING,
                                       Ids::LEAF_TEXTURE_NAME_TSETTING_DESCRIPTION));

    createArithmetic<IntValue>(_cfg, "material.leaves_atlas_res_x", 1, 100, 3,
                               &_ch.leaves_atlas_res_x)
        .setUserData(createDescription(dsc,
                                       Ids::LEAF_ATLAS_RES_X_TSETTING,
                                       Ids::LEAF_ATLAS_RES_X_TSETTING_DESCRIPTION));
    createArithmetic<IntValue>(_cfg, "material.leaves_atlas_res_y", 1, 100, 3,
                               &_ch.leaves_atlas_res_y)
        .setUserData(createDescription(dsc,
                                       Ids::LEAF_ATLAS_RES_Y_TSETTING,
                                       Ids::LEAF_ATLAS_RES_Y_TSETTING_DESCRIPTION));

    createEnum<V2::EnumStringValue>(
        _cfg,
        "material.leaves_shape",
        { "diamond", "square" },
        "diamond",
        &leaves_shape,
        (int*) &_ch.leaves_shape)
        .setUserData(createDescription(dsc,
                                       Ids::LEAF_SHAPE_TSETTING,
                                       Ids::LEAF_SHAPE_TSETTING_DESCRIPTION));

    createEnum<V2::EnumStringValue>(
        _cfg,
        "smoothing_algorithm",
        { "fast", "precise", "experimental" },
        "fast",
        &smoothing_algorithm,
        (int*) &_ch.smoothing_algorithm)
        .setUserData(createDescription(dsc,
                                       Ids::SMOOTHING_ALGORITHM_TSETTING,
                                       Ids::SMOOTHING_ALGORITHM_TSETTING_DESCRIPTION));

    _cfg.load();
}

