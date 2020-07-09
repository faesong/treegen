#pragma once

namespace VcppBits {
namespace Translation {
enum Ids : size_t {
TRANSLATION_LANGUAGE_NAME, // English
LANGUAGE_SETTING, // Language

SETTINGS_CATEGORY_GENERAL, // General
SETTINGS_RESET_TO_DEFAULT, // Reset to default

TREE_PRESET_SETTING, // Preset
SSAO_SETTING, // SSAO
SSAO_SETTING_DESCRIPTION, // Screen-space ambient occlusion
FXAA_SETTING, // FXAA
FXAA_SETTING_DESCRIPTION, // (Relatively) low-budget anti-aliasing
VIBRANCE_SETTING, // Vibrance
VIBRANCE_SETTING_DESCRIPTION, // Additinal color saturation
SHADOW_INTENSITY_SETTING, // Shadows intensity
AUTO_EXPOSURE_SETTING, // Auto exposure
AUTO_EXPOSURE_SETTING_DESCRIPTION, // Automatic brightness adjustment
AMBIENT_COLOR_SETTING, // Ambient color
AMBIENT_COLOR_SETTING_DESCRIPTION, // Objects \"in shadow\" color
FOG_COLOR_SETTING, // Fog color
FOG_COLOR_SETTING_DESCRIPTION, // Sky and atmosphere color
LIGHT_DIRECTION_SETTING, // Light direction

// Tree settings
LEVELS_TSETTING, // Levels
LEVELS_TSETTING_DESCRIPTION, // Levels of tree branches hierarchy
SEED_TSETTING, // Random seed
CURVATURE_THRESHOLD_TSETTING, // Curvature threshold
CURVATURE_THRESHOLD_TSETTING_DESCRIPTION, // Angle below which to collapse crosscut branch subdivisions, supposed to help keeping polygon count lower
ROOT_LENGTH_TSETTING, // Tree base branch length
SMOOTHING_ITERATIONS_TSETTING, // Smoothing iterations
SMOOTHING_ITERATIONS_TSETTING_DESCRIPTION, // Tree branches smoothness, costs more polygons
WIREFRAME_TSETTING, // Show wireframe
ROOT_RADIUS_TSETTING, // Tree base radius
LEAVES_LEVEL_TSETTING, // Leaves levels
LEAVES_LEVEL_TSETTING_DESCRIPTION, // Number of outer branch levels to put leaves on
LEAVES_SIZE_TSETTING, // Leaf size
LEAVES_NORMALS_OUTWARDS_TSETTING, // Direct leaves normals outside
LEAVES_NORMALS_OUTWARDS_TSETTING_DESCRIPTION, // Makes lighting of tree crown look like a big puffy object, not like multiple noizy sole leaves
LEAVES_PER_METER_TSETTING, // Number of leaves (per meter)
LEAVES_PER_METER_TSETTING_DESCRIPTION, // Leaves are spread as N per meter of a branch they are spread along
LEAVES_OFFSET_TSETTING, // Leaves spread offset
LEAVES_OFFSET_TSETTING_DESCRIPTION, // Skip some percentage (starting from root) of the branch before spreading the leaves
ACACIA_TSETTING, // Umbrella crown
ACACIA_TSETTING_DESCRIPTION, // Move all children endpoints to same height, supposed to help making african acacia type of crowns
SEGMENTS_TSETTING, // Segments number
SEGMENTS_TSETTING_DESCRIPTION, // Number of internal segments that the branch curves are interpolated from
ENDPOINT_RADIUS_MULTIPLIER_TSETTING, // Branch end radius multiplier 
CHILDREN_PER_METER_TSETTING, // Children per meter
CHILDREN_MIN_TSETTING, // Children minumum number
OFFSET_TSETTING, // Children offset
OFFSET_SHORTBRANCH_CANCEL_TSETTING, // Cancel offset on short branches
OFFSET_SHORTBRANCH_CANCEL_TSETTING_DESCRIPTION, // Supposed to keep crown populated with children on thin parts of the crown, so. Also helps to keep polycount lower when combined with higher Children offset setting
ENDPOINT_DIRECTION_RANDOM_TSETTING, // Endpoint direction - random
ENDPOINT_DIRECTION_OUTWARD_TSETTING, // Endpoint direction - outward 
ENDPOINT_DIRECTION_OUTWARD_TSETTING_DESCRIPTION, // Away from the parent branch
ENDPOINT_DIRECTION_ZWARD_TSETTING, // Endpoint direction - upward
ENDPOINT_DIRECTION_CROWN_OUTWARD_TSETTING, // Endpoint direction - crown center outward
ENDPOINT_DIRECTION_CROWN_OUTWARD_TSETTING_DESCRIPTION, // Supposed to help achieving spherical type of crowns
ENDPOINT_DIRECTION_PARENT_TSETTING, // Endpoint direction - parent
ENDPOINT_DIRECTION_PARENT_TSETTING_DESCRIPTION, // Repeat parent direction
ENDPOINT_INFLUENCE_TSETTING, // Endpoint direction - total
ENDPOINT_INFLUENCE_TSETTING_DESCRIPTION, // Convenience option to disable whole Endpoint settings
GROWTH_DIRECTION_DEVIATION_TSETTING, // Growth direction deviation
GROWTH_DIRECTION_DEVIATION_TSETTING_DESCRIPTION, // Randomize direction of each next generated segments
LENGTH_MULTIPLIER_TSETTING, // Length multiplier
LENGTH_MULTIPLIER_TSETTING_DESCRIPTION, //
LENGTH_DISTRIBUTION_CIRCLE_TSETTING, // Length distribution - circle
LENGTH_DISTRUBUTION_CIRCLE_TSETTING_DESCRIPTION, // Makes round crown
LENGTH_DISTRIBUTION_CONE_TSETTING, // Length distribution - cone
LENGTH_DISTRIBUTION_CONE_TSETTING_DESCRIPTION, // Makes spruce type of crown
LENGTH_DISTRIBUTION_FLAME_TSETTING, // Length distribution - flame
LENGTH_DISTRIBUTION_FLAME_TSETTING_DESCRIPTION, //
LENGTH_DISTRIBUTION_RANDOM_TSETTING, // Length distribution - random
LENGTH_DISTRIBUTION_RANDOM_TSETTING_DESCRIPTION, //
RADIUS_MULTIPLIER_TSETTING, // Radius multiplier
RADIUS_MULTIPLIER_TSETTING_DESCRIPTION, // Branch radius relative to parent branch
DIRECTION_OUTWARD_TSETTING, // Direction - outward
DIRECTION_OUTWARD_TSETTING_DESCRIPTION, // Perpendicular to parent branch
DIRECTION_UPWARD_TSETTING, // Direction - upward
DIRECTION_UPWARD_TSETTING_DESCRIPTION, //
DIRECTION_DEVIATION_TSETTING, // Randomize direction
DIRECTION_DEVIATION_TSETTING_DESCRIPTION, //
DIRECTION_PARENT_TSETTING, // Direction - parent
DIRECTION_PARENT_TSETTING_DESCRIPTION, //
DIRECTION_CROWN_OUTWARD_TSETTING, // Direction - crown-outward
DIRECTION_CROWN_OUTWARD_TSETTING_DESCRIPTION, // For spherical crown
CROWN_POS_OFFSET_TSETTING, // Crown position offset
CROWN_POS_OFFSET_TSETTING_DESCRIPTION, // Changes the nominal center point of the crown, which some other settings affected by 
LEAF_DIRECTION_OUTWARD_TSETTING, // Leaves direction - outward
LEAF_DIRECTION_OUTWARD_TSETTING_DESCRIPTION, // Perpendicular to the parent branch
LEAF_DIRECTION_UPWARD_TSETTING, // Leaves direction - upward
LEAF_DIRECTION_UPWARD_TSETTING_DESCRIPTION, // 
LEAF_DIRECTION_RANDOM_TSETTING, // Leaves direction - random
LEAF_DIRECTION_RANDOM_TSETTING_DESCRIPTION, // 
LEAF_DIRECTION_PARENT_TSETTING, // Leaves direction - parent
LEAF_DIRECTION_PARENT_TSETTING_DESCRIPTION, // Copy parent's direction
LEAF_NORMAL_PARENT_TSETTING, // Leaves normals - parent
LEAF_NORMAL_PARENT_TSETTING_DESCRIPTION, // 
LEAF_NORMAL_OUTWARD_TSETTING, // Leaves normals - outward
LEAF_NORMAL_OUTWARD_TSETTING_DESCRIPTION, // 
LEAF_NORMAL_UPWARD_TSETTING, // Leaves normals - upward
LEAF_NORMAL_UPWARD_TSETTING_DESCRIPTION, //
LEAF_TEXTURE_NAME_TSETTING, // Leaves texture name
LEAF_TEXTURE_NAME_TSETTING_DESCRIPTION, // 
LEAF_ATLAS_RES_X_TSETTING, // Leaves X atlas resolution
LEAF_ATLAS_RES_X_TSETTING_DESCRIPTION, // 
LEAF_ATLAS_RES_Y_TSETTING, // Leaves Y atlas resolution
LEAF_ATLAS_RES_Y_TSETTING_DESCRIPTION, // 
LEAF_SHAPE_TSETTING, // Leaves shape
LEAF_SHAPE_TSETTING_DESCRIPTION, // 
SMOOTHING_ALGORITHM_TSETTING, // Smoothing algorithm
SMOOTHING_ALGORITHM_TSETTING_DESCRIPTION, // 
BRANCH_MIRROR_DIRECTION_TSETTING, // Mirror direction on child split
BRANCH_MIRROR_DIRECTION_TSETTING_DESCRIPTION, // Makes parent branch to grow opposite to the direction of the child after split
_ILLEGAL_ELEMENT_, //
};
}
}
