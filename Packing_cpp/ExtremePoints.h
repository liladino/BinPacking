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
		const auto farthestXYZ = getCurrentExtent(true);
		const auto originalExtent = item.extent;
		
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
				foundPlace = true;

				Vec3 newExtent = {
					std::max(farthestXYZ[0], candidate[0] + originalExtent[0]),
					std::max(farthestXYZ[1], candidate[1] + originalExtent[1]),
					std::max(farthestXYZ[2], candidate[2] + originalExtent[2])
				};
				unsigned long long newBoundingBox = newExtent[0] * newExtent[1] * newExtent[2];

				if (newBoundingBox < bestBoundingBox || 
						(newBoundingBox == bestBoundingBox && comparator(candidate, item.getPos()))){
					bestBoundingBox = newBoundingBox;
					bestPos = candidate;
				}
			}
		}
		item.setPos(bestPos);
		if (!foundPlace){
			item = copy;
		}
		return foundPlace;
	}

	void pruneExtremePoints(){
		std::vector<std::set<Vec3, CompareEP>::iterator> toRemove;

		auto insideItem = [&](Vec3 point){
			if (point[0] >= binSize[0] || point[1] >= binSize[1] || point[2] >= binSize[2]) {
				return true;
			}
			for (const auto& item : packed){
				if (item.getPos()[0] < point[0] && item.getPos()[0] + item[0] > point[0] &&
				    item.getPos()[1] < point[1] && item.getPos()[1] + item[1] > point[1] &&
				    item.getPos()[2] < point[2] && item.getPos()[2] + item[2] > point[2]) {
					return true;
				}
			}
			return false;
		};

		for (auto it = extremePoints.begin(); it != extremePoints.end(); ++it){
			if (insideItem(*it)){
				toRemove.push_back(it);
			}
		}
		for (const auto& y : toRemove){
			extremePoints.erase(y);
		}
	}

	void addNewExtremePoints(){
		const auto& lastItem = packed[packed.size()-1]; 
		//corner points
		extremePoints.insert({lastItem.getPos()[0] + lastItem[0], lastItem.getPos()[1],               lastItem.getPos()[2]              }); 
		extremePoints.insert({lastItem.getPos()[0],               lastItem.getPos()[1] + lastItem[1], lastItem.getPos()[2]              });
		extremePoints.insert({lastItem.getPos()[0],               lastItem.getPos()[1],               lastItem.getPos()[2] + lastItem[2]});

		//
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

		addNewExtremePoints();
		pruneExtremePoints();
		
		return true;
	}
};


#endif