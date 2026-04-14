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
		size_t x1 = 0, x2 = 0;
		size_t pos_y = 0;
	};

	struct IRotateAlongZ {
		virtual void rotateAlongZ(Item& item) = 0;
	};

	class Shelf2DPacker {
		struct Shelf {
			size_t x_limit = 0;
			size_t y_limit = 0;
			std::vector<Space> spaces = {};
		};
		std::vector<Shelf> shelves = {};

		size_t z;
		size_t layerHeight = 0;
		Vec3 binSize;
		
		void addFreeSpaceOver(size_t currentShelf, const Item& item){
			shelves[currentShelf].spaces.push_back({ item.getPos()[0], item.getPos()[0] + item[0], item.getPos()[1] + item[1] });
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
			shelves = {};
			layerHeight = 0;
		}

		bool pack(Item& item) {			
			if (item[0] < item[1]){
				std::swap(item[0], item[1]);
			}

			//try to fit in free space
			for (size_t i = 0; i < shelves.size(); i++){
				for (Space& space : shelves[i].spaces) {
					size_t x = space.x2 - space.x1;
					size_t y = shelves[i].y_limit - space.pos_y;

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
			if (shelves.size() == 0){
				size_t itemx = item[0], itemy = item[1];
				for (int i = 0; i < 2; i++){
					if (itemx <= binSize[0] && itemy <= binSize[1] && z + item[2] <= binSize[2]){
						if (i == 1){
							std::swap(item[0], item[1]);
						}

						shelves.push_back({item[0], item[1], {}});

						placeItem(0, 0, 0, item);
						return true;
					}
					//rotate the item along the z axis to try to fit
					std::swap(itemx, itemy);
				}
				return false; 
			}
			
			//try to pack at the end of a shelf
			for (size_t i = 0; i < shelves.size(); i++){
				size_t x = shelves[i].x_limit, 
					   y = (i > 0 ? shelves[i-1].y_limit : 0); //set y to be on top of the last shelf

				bool isShelfFlexible = (i == shelves.size()-1);

				for (int j = 0; j < 2; j++){
					size_t itemx = item[0], itemy = item[1];
					if (x + itemx <= binSize[0] && y + itemy <= binSize[1] && z + item[2] <= binSize[2]){
						if (1 == j) std::swap(item[0], item[1]);
						placeItem(x, y, i, item);
						if (isShelfFlexible){
							shelves[i].y_limit = std::max(y + item[1], shelves[i].y_limit); //the current shelf depth is not yet fixed, no shelves after
						}
						shelves[i].x_limit = x + item[0];
						return true;
					}
					//rotate the item along the z axis to try to fit
					std::swap(itemx, itemy);
				}
			}

			//could'nt put it any shelf, try to open a new shelf
			size_t lastShelfIndex = shelves.size()-1;
			if (shelves[lastShelfIndex].x_limit > 0){ //only try if the last shelf was not empty, otherwise wouldn't help to add a shelf
				size_t y = (lastShelfIndex > 0 ? shelves[lastShelfIndex-1].y_limit : 0);

				size_t itemx = item[0], itemy = item[1];
				for (int i = 0; i < 2; i++){
					if (itemx <= binSize[0] && y + itemy <= binSize[1] && z + item[2] <= binSize[2]){
						if (1 == i){
							std::swap(item[0], item[1]);
						}
						shelves.push_back({item[0], y + item[1], {}});
						placeItem(0, y, lastShelfIndex+1, item);
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
