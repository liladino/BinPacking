#ifndef SHELFPACKER_H
#define SHELFPACKER_H

#include "Packer.h"
#include "Shelf2D.h"
#include "IRotationPolicy.h"

class ShelfPacker : public Packer {
	std::unique_ptr<IRotationPolicy> rotationPolicy;
    size_t currentZ, currentIndex;
    std::vector<std::pair<Shelf2D::Shelf2DPacker, size_t>> packers2d; //list of 2d packers, each of which has a Z starting point
	
    void init(){
		packed = {};
		binSize = {1, 1, 1};
		currentZ = 0;
	}	
public:
	ShelfPacker() {
        rotationPolicy = std::make_unique<RP_largestFaceUp>();
		init();
	}

    /* Try to put into the first layer that accepts it */
	virtual bool pack(Item toPack) override {
        rotationPolicy->rotateBin(toPack, binSize);
        currentIndex = currentZ = 0;
        while (currentZ < binSize[2]){
            if (packers2d.size() <= currentIndex) {
                packers2d.push_back({Shelf2D::Shelf2DPacker(currentZ, binSize), currentZ});
            }
            if (currentZ + toPack[2] > binSize[2]){
                return false;
            }

            if (/* TODO: layer above exist, ceiling = layerAbove && currentZ + toPack[2] <= ceiling*/ true){
                if (packers2d[currentIndex].first.pack(toPack)){
                    packed.push_back(toPack);
                    return true;
                }
            }
            currentZ = packers2d[currentIndex].second;
            currentIndex++;
        }
        return false;
	}

    virtual void setLimits(size_t x, size_t y, size_t z) override { 
		Packer::setLimits(x, y, z);
        for (std::pair<Shelf2D::Shelf2DPacker, size_t>& p : packers2d){
            p.first.setBinSize({x, y, z});
        }
	}
};



#endif