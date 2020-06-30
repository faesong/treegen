#pragma once


#include <utility>


#include <VcppBits/Translation/Translation.hpp> // TODO: refactor the needed Ids
                                                // to separate file?


using TranslationIdsPair = std::pair<VcppBits::Translation::Ids,
                                     VcppBits::Translation::Ids>;

inline TranslationIdsPair*
createDescription (std::deque<TranslationIdsPair> &pDest,
                   const VcppBits::Translation::Ids pNameId,
                   const VcppBits::Translation::Ids pDescriptionId
                       = VcppBits::Translation::Ids::_ILLEGAL_ELEMENT_) {
    pDest.push_back({pNameId, pDescriptionId});
    return &pDest.back();
}
