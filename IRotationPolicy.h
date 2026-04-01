#ifndef IROTATIONPOLICY_H
#define IROTATIONPOLICY_H

#include "Item.h"
#include <array>
#include <algorithm>
#include <functional>
#include <climits>

struct IRotationPolicy {
	/* toPack: rotate this Bin. It's intended position should be loaded.
	 * limits: dimensions of the current container
	 */
	virtual void rotateBin(Item& toPack, const Vec3& binSize) = 0;
};

struct RP_largestFaceUp : IRotationPolicy {
	/* Order the edges of the bin into a decreasing order.
	 * This way, the largest face will be x-y (and such the smallest will be height z)
	 */
	void rotateBin(Item& toPack, const Vec3& binSize) override {
		std::sort(toPack.extent.begin(), toPack.extent.end(), std::greater<size_t>());
	}
};

struct RP_tryFirstFitting : IRotationPolicy {
	/* Try out the first 5 of the 6 directions, and return the first
     * which fits within the limits. If none fits, return the 6th 
     * without check - the caller checks it anyway.
	 */
	void rotateBin(Item& toPack, const Vec3& binSize) override {
		const static Vec3 rotations[] = 
			{   
				{0, 2, 1},
				{1, 0, 2},
				{1, 2, 0},
				{2, 0, 1},
				{2, 1, 0},
				{0, 1, 2},
			};
		
		for (size_t i = 0; i < 5; i++) {
            bool fits = true;
			for (size_t j = 0; j < 3; j++) {
				size_t src = rotations[i][j];
				long long dim = toPack[src];

				if (binSize[j] < toPack.getPos(j) + dim) {
                    fits = false;
                    break;
                }
            }
            if (fits) {
                /* all dimensions were good, rotate bin
                 */
                auto extent = toPack.extent;
                toPack[0] = extent[rotations[i][0]];
                toPack[1] = extent[rotations[i][1]];
                toPack[2] = extent[rotations[i][2]];
                return;
            }
		}
        /* If no rotation fits, return the 6th, unchecked orientation.
         * To save computation, the 6th in the order is the original order,
         * so no job here.
         */
	}
};

struct RP_minLeftoverSlack : IRotationPolicy {
	/* Order the edges of the bin to minimize leftover slack, 
	 * relative to the current container.
	 */	 
	void rotateBin(Item& toPack, const Vec3& binSize) override {
		const static Vec3 rotations[] = 
			{   
				{0, 1, 2},
				{0, 2, 1},
				{1, 0, 2},
				{1, 2, 0},
				{2, 0, 1},
				{2, 1, 0},
			};

		long long bestSlack = LLONG_MAX;
		size_t bestIdx = 0;
		
		for (size_t i = 0; i < 6; i++) {
			long long slack = 0;
			/* slack = leftover space in dimensions
			 *     = sum_dimension_k(
			 *           limit_k - position_k + size_k
			 *       )
             * where size_k is the dimension in that dimension (rotation!)
			 */

			for (size_t j = 0; j < 3; j++) {
				size_t src = rotations[i][j];
				long long dim = toPack[src];
				long long slackj = binSize[j] - (toPack.getPos(j) + dim);

				if (slackj < 0) {
					slack = LLONG_MAX;
					break;
				}
				slack += slackj;
			}

			if (slack < bestSlack) {
				bestSlack = slack;
				bestIdx = i;
			}
		}

		/* rotate bin according to min slack */
		auto extent = toPack.extent;
		toPack[0] = extent[rotations[bestIdx][0]];
		toPack[1] = extent[rotations[bestIdx][1]];
		toPack[2] = extent[rotations[bestIdx][2]];
	}
};

#endif