#ifndef GREEDYPACKER_H
#define GREEDYPACKER_H

#include "Item.h"
#include "Packer.h"
#include <memory>

class GreedyPacker : public Packer {
	struct ComparePOI {
		bool operator()(const Vec3& a, const Vec3& b) const {
			if (a[2] != b[2]) return a[2] < b[2]; // z
			if (a[1] != b[1]) return a[1] < b[1]; // y
			return a[0] < b[0];                   // x
		}
	};
	std::set<Vec3, ComparePOI> pointsOfInterest;
	void addCornersToPOI(const Item& toPack){
		auto& pos = toPack.getPos(); 
		pointsOfInterest.insert({pos[0] + toPack[0], pos[1], pos[2]}); 
		pointsOfInterest.insert({pos[0], pos[1] + toPack[1], pos[2]});
		pointsOfInterest.insert({pos[0], pos[1], pos[2] + toPack[2]});
	}
	void init(){
		packed = {};
		pointsOfInterest.insert(Vec3(0, 0, 0));
		binSize = {1, 1, 1};
	}
public:
	GreedyPacker(){
		init();
	}
	GreedyPacker(std::unique_ptr<IRotationPolicy> rotationPolicy) {
		init();
		this->rotationPolicy = std::move(rotationPolicy);
	}

	virtual bool pack(Item toPack) override {
		for (auto it = pointsOfInterest.begin(); it != pointsOfInterest.end(); ++it) {
			toPack.setPos(*it);

			if (nullptr != rotationPolicy){
				rotationPolicy->rotateBin(toPack, binSize);
			}

			if (!fitsWithinLimits(toPack)) continue;
			if (intersectsAny(toPack)) continue;

			packed.push_back(toPack);
			addCornersToPOI(toPack);
			pointsOfInterest.erase(it);
			
			return true;
		}
		return false;
	}
};


#endif