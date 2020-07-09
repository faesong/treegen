#pragma once


#include <utility>


#include "TranslationIds.hpp"


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
