#ifndef GREEDYPACKER_H
#define GREEDYPACKER_H

#include "Bin.h"
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

	std::unique_ptr<IRotationPolicy> rotationPolicy;
	void init(){
		packed = {};
		pointsOfInterest.insert(Vec3(0, 0, 0));
		limits = {1, 1, 1};
	}
public:
	GreedyPacker(){
		init();
	}
	GreedyPacker(std::unique_ptr<IRotationPolicy> rotationPolicy) : rotationPolicy(std::move(rotationPolicy)) {
		init();
	}
	void setPolicy(std::unique_ptr<IRotationPolicy> rotationPolicy) {
		this->rotationPolicy = std::move(rotationPolicy);
	}
	
	void updatePOI(const Bin3& toPack){
		auto& pos = toPack.getPos(); 
		pointsOfInterest.insert({pos[0] + toPack[0], pos[1], pos[2]}); 
  	  	pointsOfInterest.insert({pos[0], pos[1] + toPack[1], pos[2]});
   		pointsOfInterest.insert({pos[0], pos[1], pos[2] + toPack[2]});
	}

	virtual bool pack(Bin3 toPack) override {
		for (auto it = pointsOfInterest.begin(); it != pointsOfInterest.end(); ++it) {
			toPack.setPos(*it);

			if (nullptr != rotationPolicy){
				rotationPolicy->rotateBin(toPack, limits);
			}

			if (!fitsWithinLimits(toPack)) continue;
			if (intersectsAny(toPack)) continue;

			packed.push_back(toPack);
			updatePOI(toPack);
			pointsOfInterest.erase(it);
			
			return true;
		}
		return false;
	}
};


#endif