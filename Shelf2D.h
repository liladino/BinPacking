#ifndef SHELF2D_h
#define SHELF2D_h

#include "Item.h"
#include "Packer.h"
#include <vector>
#include <memory>

/*
2D Heuristic

Example 1.
item1 is 1 by 1. We have 1 shelf.
+-+
|1|
+-+

item2 is say 3 by 3. Because the current shelf has a current height 1, no option but to put it next to it.
  +---+
  |   |
._| 2 |
|1|   |
+-+---+
the shelf height is 3 now.

Lets say then comes a 1 by 2 item.
then it exactly fits in the space left by item1.
+-+---+
|3|   |
|_| 2 |
|1|   |
+-+---+

Example 2:

------------
occupied shelf here
-------+---+
       |   |
.______|   |
|      |   |
+------+---+

The lower shelf (6x1 and 3x3) produces 2 free spaces:
(0,1)-(5,1) and (5,3 and 8,3)
the height is 3, and because there already is a shelf over it, the second free space has 0 area.  

The first is a true free space:
0,1 - 5,1 - 5,3 - 0,3

if a 2x1 item arrives:
------------
occupied shelf
-------+---+
.__.   |   |
|__|___|   |
|      |   |
+------+---+

the free spaces become:
(0,2)-(2,2) and (2,1)(5,1) and (5,3 and 8,3).

*/

namespace Shelf2D {
	/*
	 * To get the free space over item: (x2 - x1) by (shelfLimit_y - usedExtent_y). 
	 */
	struct Space {
		size_t x1, x2;
		size_t pos_y;
		Space(){x1 = x2 = pos_y = 0; }
		Space(size_t x1, size_t x2, size_t pos_y) : x1(x1), x2(x2), pos_y(pos_y) { }
	};

	struct IRotateAlongZ {
		virtual void rotateAlongZ(Item& item) = 0;
	};

	class Shelf2DPacker {
		std::vector<size_t> shelfLimit_x = {};
		std::vector<size_t> shelfLimit_y = {};
		std::vector<std::vector<Space>> freeSpaceOver = {};
		size_t z;
		size_t layerHeight = 0;
		Vec3 binSize;
		
		void addFreeSpaceOver(size_t currentShelf, const Item& item){
			freeSpaceOver[currentShelf].push_back({ item.getPos()[0], item.getPos()[0] + item[0], item.getPos()[1] + item[1] });
		}

		/* Updates the free space if an item was put there.
		 * moves the free space, and makes a new free space
		 * over the item.
		 */
		void updateFreeSpaces(size_t currentShelf, Space& space, const Item& item){
			space.x1 += item[0];
			addFreeSpaceOver(currentShelf, item);
		}

		void placeItemInFreeSpace(size_t currentShelf, Space& space, Item& item){			
			item.setPos({space.x1, space.pos_y, z});
			layerHeight = std::max(item[2], layerHeight);
			updateFreeSpaces(currentShelf, space, item);
		}

		void placeItem(size_t x, size_t y, size_t shelf, Item& item){
			item.setPos({x, y, z});
			layerHeight = std::max(item[2], layerHeight);
			addFreeSpaceOver(shelf, item);
		}

	public:
		Shelf2DPacker(size_t z, Vec3& size) : z(z), binSize(size) {	reset(); }
		Shelf2DPacker(size_t z) : z(z) { reset(); }

		void setBinSize(Vec3& size) { 
			binSize = size; 
		}

		size_t getLayerHeight() {
			return layerHeight;
		}
		
		void reset() {
			shelfLimit_x = {};
			shelfLimit_y = {};
			freeSpaceOver = {};
			layerHeight = 0;
		}

		bool pack(Item& item) {			
			if (item[0] < item[1]){
				std::swap(item[0], item[1]);
			}

			//try to fit in free space
			for (size_t i = 0; i < freeSpaceOver.size(); i++){
				for (Space& space : freeSpaceOver[i]) {
					size_t x = space.x2 - space.x1;
					size_t y = shelfLimit_y[i] - space.pos_y;

					if (item[0] <= x && item[1] <= y) {
						placeItemInFreeSpace(i, space, item);
						return true;
					}

					//try fit rotated 90deg around z
					if (item[0] <= y && item[1] <= x) {
						std::swap(item[0], item[1]);
						placeItemInFreeSpace(i, space, item);
						return true;
					}
				}
			}

			// nothing was placed yet
			if (shelfLimit_y.size() == 0){
				size_t itemx = item[0], itemy = item[1];
				for (int i = 0; i < 2; i++){
					if (itemx <= binSize[0] && itemy <= binSize[1] && z + item[2] <= binSize[2]){
						if (i == 1){
							std::swap(item[0], item[1]);
						}

						shelfLimit_x.push_back(item[0]); 
						shelfLimit_y.push_back(item[1]); 
						placeItem(0, 0, 0, item);
						return true;
					}
					//rotate the item along the z axis to try to fit
					std::swap(itemx, itemy);
				}
				return false; 
			}
			
			//try to pack at the end of a shelf
			for (size_t i = 0; i < shelfLimit_x.size(); i++){
				size_t x = shelfLimit_x[i], 
					   y = (i > 0 ? shelfLimit_y[i-1] : 0); //set y to be on top of the last

				for (int j = 0; j < 2; j++){
					if (x + item[0] <= binSize[0] && y + item[1] <= binSize[1] && z + item[2] <= binSize[2]){
						placeItem(x, y, i, item);
						if (i == shelfLimit_y.size()-1){
							shelfLimit_y[i] = std::max(y + item[1], shelfLimit_y[i]); //the current shelf depth is not yet fixed, no shelves yetet after
						}
						shelfLimit_x[i] = x + item[0];
						return true;
					}
					//rotate the item along the z axis to try to fit
					std::swap(item[0], item[1]);
				}
			}

			//could'nt put it any shelf, try to open a new shelf
			size_t lastShefIndex = shelfLimit_x.size()-1;
			if (shelfLimit_x[lastShefIndex] != 0){
				//only try if the last shelf was not empty
				size_t y = (lastShefIndex > 0 ? shelfLimit_y[lastShefIndex-1] : 0);

				size_t itemx = item[0], itemy = item[1];
				for (int i = 0; i < 2; i++){
					if (itemx <= binSize[0] && y + itemy <= binSize[1] && z + item[2] <= binSize[2]){
						if (1 == i){
							std::swap(item[0], item[1]);
						}
						shelfLimit_x.push_back(item[0]); 
						shelfLimit_y.push_back(y + item[1]); 
						placeItem(0, y, lastShefIndex+1, item);
						return true;
					}
					//rotate the item along the z axis to try to fit
					std::swap(itemx, itemy);
				}
			}

			return false;
		}
	};
}


#endif
