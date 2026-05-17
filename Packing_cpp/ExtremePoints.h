#ifndef EXTREMEPOINTS_H
#define EXTREMEPOINTS_H

#include "Item.h"
#include "Packer.h"

class ExtremePointsPacker : public Packer {
	struct CompareEP {
		bool operator()(const Vec3& a, const Vec3& b) const {
			if (a[2] != b[2]) return a[2] < b[2]; // z
			if (a[1] != b[1]) return a[1] < b[1]; // y
			return a[0] < b[0];                   // x
		}
	};
	std::set<Vec3, CompareEP> extremePoints;
	void init(){
		packed = {};
		extremePoints.clear(); 
		extremePoints.insert(Vec3(0, 0, 0));
		binSize = {1, 1, 1};
	}

	static constexpr std::array<Vec3, 6> rotations = 
		{{
			{0, 1, 2},
			{0, 2, 1},
			{1, 0, 2},
			{1, 2, 0},
			{2, 0, 1},
			{2, 1, 0}
		}};

	bool tryAllPoints(Item& item){
		CompareEP comparator;

		Item copy = item;
		unsigned long long bestBoundingBox = LLONG_MAX;
		Vec3 bestPos;
		size_t bestRotation;
		const auto farthestXYZ = getCurrentExtent(true);
		std::array<size_t, 3> originalExtent = item.extent;
		
		auto rotate = [&](size_t i){
			item[0] = originalExtent[rotations[i][0]];
			item[1] = originalExtent[rotations[i][1]];
			item[2] = originalExtent[rotations[i][2]];
		};

		bool foundPlace = false;
		for (const auto& candidate : extremePoints) {
			for (size_t i = 0; i < 6; i++){
				rotate(i);
				item.setPos(candidate); 
				if (!fitsWithinLimits(item)) continue;
				if (intersectsAny(item)) continue;

				Vec3 newExtent = {
					std::max(farthestXYZ[0], candidate[0] + originalExtent[0]),
					std::max(farthestXYZ[1], candidate[1] + originalExtent[1]),
					std::max(farthestXYZ[2], candidate[2] + originalExtent[2])
				};
				unsigned long long newBoundingBox = newExtent[0] * newExtent[1] * newExtent[2];

				if (newBoundingBox < bestBoundingBox || 
						(newBoundingBox == bestBoundingBox && comparator(candidate, item.getPos()))){
					foundPlace = true;
					bestBoundingBox = newBoundingBox;
					bestPos = candidate;
					bestRotation = i;
				}
			}
		}
		if (!foundPlace){
			item = copy;
			return false;
		}

		rotate(bestRotation);
		item.setPos(bestPos);
		
		return true;
	}

	void updateExtremePoints(const Item& lastItem) {
		std::vector<std::set<Vec3, CompareEP>::iterator> eclipsed;
		
		// 1. Find all Extreme Points eclipsed by the new item
		// A point is eclipsed if it falls inside or on the inner boundary of the item.
		for (auto it = extremePoints.begin(); it != extremePoints.end(); ++it) {
			const Vec3& p = *it;
			if (p[0] >= lastItem.getPos(0) && p[0] < lastItem.getPos(0) + lastItem[0] &&
				p[1] >= lastItem.getPos(1) && p[1] < lastItem.getPos(1) + lastItem[1] &&
				p[2] >= lastItem.getPos(2) && p[2] < lastItem.getPos(2) + lastItem[2]) {
				eclipsed.push_back(it);
			}
		}
		
		// 2. Project eclipsed points to the far faces of the new item
		std::vector<Vec3> newPoints;
		for (auto it : eclipsed) {
			const Vec3& p = *it;
			newPoints.push_back({lastItem.getPos(0) + lastItem[0], p[1], p[2]}); // X-axis projection
			newPoints.push_back({p[0], lastItem.getPos(1) + lastItem[1], p[2]}); // Y-axis projection
			newPoints.push_back({p[0], p[1], lastItem.getPos(2) + lastItem[2]}); // Z-axis projection
		}
		
		// 3. Erase the eclipsed points
		for (auto it : eclipsed) {
			extremePoints.erase(it);
		}
		
		// 4. Filter and add valid new points
		for (const auto& np : newPoints) {
			// Drop points touching or outside the far bin limits
			if (np[0] >= binSize[0] || np[1] >= binSize[1] || np[2] >= binSize[2]) {
				continue;
			}
			
			// Drop points strictly inside any already packed item
			bool inside = false;
			for (const auto& item : packed) {
				if (np[0] > item.getPos(0) && np[0] < item.getPos(0) + item[0] &&
					np[1] > item.getPos(1) && np[1] < item.getPos(1) + item[1] &&
					np[2] > item.getPos(2) && np[2] < item.getPos(2) + item[2]) {
					inside = true;
					break;
				}
			}
			
			if (!inside) {
				extremePoints.insert(np);
			}
		}
	}

public:
	ExtremePointsPacker(){
		init();
	}
	
    void clear() override {
        init();
    }

	virtual bool pack(Item& toPack) override {
		if (!tryAllPoints(toPack)){
			return false;
		}
		packed.push_back(toPack);
		updateExtremePoints(toPack);
		return true;
	}
};


#endif