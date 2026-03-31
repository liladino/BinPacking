#ifndef IROTATIONPOLICY_H
#define IROTATIONPOLICY_H

#include "bin.h"
#include <array>
#include <algorithm>
#include <functional>

struct IRotationPolicy {
    /* toPack: rotate this Bin. It's intended position should be loaded.
     * limits: dimensions of the current container
     */
    virtual void rotateBin(Bin3& toPack, const Vec3& limits) = 0;
};

struct RP_largestFaceUp : IRotationPolicy {
    void rotateBin(Bin3& toPack, const Vec3& limits){
        // std::array<size_t, 3> extent = {toPack[0], toPack[1], toPack[2]};
        std::sort(toPack.extent.begin(), toPack.extent.end(), std::greater<size_t>());
        // toPack[0] = extent[0];
        // toPack[1] = extent[1];
        // toPack[2] = extent[2];
    }
};

#endif