#ifndef SHELFPACKER_H
#define SHELFPACKER_H

namespace Shelf2D{
    struct Placement2D {
        size_t bottoMLeftX, bottoMLeftY, topRightX, topRightY.
        Item2D(){ 
            bottoMLeftX = bottoMLeftY = topRightX = topRightY = 0;
        }
        Item2D(size_t blX, size_t blY, size_t trX, size_t trY) {
            bottoMLeftX = blX;
            bottoMLeftY = blY;
            topRightX = trX;
            topRightY = trY;
        } 
    }

    class Shelf2DPacker {
        std::vector<Placement2D> currentLayer;
    public:
        void reset(){
            currentLayer = {};
        }
        bool pack(Item2D item, Placement2D& out){
            
        }
    };
}

class ShelfPacker : public Packer {
	void init(){
		packed = {};
		binSize = {1, 1, 1};
		currentZ = 0;
	}

	size_t currentZ;
public:
	ShelfPacker(){
		init();
	}
	ShelfPacker(std::unique_ptr<IRotationPolicy> rotationPolicy) {
		init();
		this->rotationPolicy = std::move(rotationPolicy);
	}

	virtual bool pack(Item toPack) override {
		throw "not implemeted";
        return false;
	}
}



#endif