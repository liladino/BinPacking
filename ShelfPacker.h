#ifndef SHELFPACKER_H
#define SHELFPACKER_H

#include "Packer.h"
#include "Shelf2D.h"
#include "IRotationPolicy.h"

class ShelfPacker : public Packer {
	void init(){
		packed = {};
		binSize = {1, 1, 1};
		currentZ = 0;
	}
	size_t currentZ;
	virtual void setPolicy(std::unique_ptr<IRotationPolicy> rotationPolicy) {}

    // Shelf2D::Shelf2DPacker packer2d;
public:
	ShelfPacker(){
        rotationPolicy = std::make_unique<RP_largestFaceUp>();
		init();
	}

	virtual bool pack(Item toPack) override {
		throw "not implemeted";
        return false;
	}
};



#endif