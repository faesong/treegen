#include "TreeGenSettings.hpp"
#include <UrhoBits/TreeGenerator/TreeGenerator.hpp> // TODO refactor to separate TreeConfig.hpp

using namespace V2; // TODO rename it?

struct TreeConfigCache {
    TreeConfigCache (Settings2&);

    UrhoBits::TreeConfig* getTreeConfig () {
        return &_ch;
    }

    std::string leafTextureName;

    std::string leaves_shape;
    std::string smoothing_algorithm;

private:
    UrhoBits::TreeConfig _ch;

    Settings2& _cfg;
};

