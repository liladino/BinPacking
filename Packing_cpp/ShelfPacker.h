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
        packers2d = {};
	}	
public:
	ShelfPacker() {
        rotationPolicy = std::make_unique<RP_largestFaceUp>();
		init();
	}

    /* Try to put into the first layer that accepts it */
	virtual bool pack(Item& toPack) override {
        rotationPolicy->rotateBin(toPack, binSize);
        currentIndex = currentZ = 0;
        if (!fitsWithinLimits(toPack)){
            return false;
        }

        while (currentZ < binSize[2]){
            if (packers2d.size() <= currentIndex) {
                packers2d.push_back({Shelf2D::Shelf2DPacker(currentZ, binSize), currentZ});
            }
            if (currentZ + toPack[2] > binSize[2]){
                return false;
            }

            size_t ceiling = binSize[2];
            if (currentIndex + 1 < packers2d.size()) {
                ceiling = packers2d[currentIndex+1].second;
            }

            //try to fit in the current layer, otherwise move up a layer
            if (currentZ + toPack[2] <= ceiling){
                if (packers2d[currentIndex].first.pack(toPack)){
                    packed.push_back(toPack);
                    return true;
                }
            }

            size_t nextZ = packers2d[currentIndex].second
                         + packers2d[currentIndex].first.getLayerHeight();

            currentIndex++;
            currentZ = nextZ;
        }
        return false;
	}

    virtual void setLimits(size_t x, size_t y, size_t z) override { 
		Packer::setLimits(x, y, z);
        for (std::pair<Shelf2D::Shelf2DPacker, size_t>& p : packers2d){
            p.first.setBinSize({x, y, z});
        }
	}

	friend struct ::ShelfTester;
};



#endif