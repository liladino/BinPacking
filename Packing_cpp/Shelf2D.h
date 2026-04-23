#ifndef SHELF2D_h
#define SHELF2D_h

#include "Item.h"
#include "test.h"
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

struct ShelfTester;

namespace Shelf2D {
	/*
	 * To get the free space over item: (x2 - x1) by (shelfLimit_y - usedExtent_y). 
	 */
	struct Space {
		size_t x1 = 0, x2 = 0;
		size_t pos_y = 0;
	};

	class Shelf2DPacker {
		struct Shelf {
			size_t x_limit = 0; //global
			size_t y_start = 0; //global
			size_t height = 0; 
			std::vector<Space> spaces = {};
		};
		std::vector<Shelf> shelves = {};
		std::vector<Item> placed;

		size_t z;
		size_t layerHeight = 0;
		Vec3 binSize;
		Item original, rotated;
		Item packed;
		
		void cleanupFreeSpace(size_t currentShelf) {
			auto& spaces = shelves[currentShelf].spaces;

			spaces.erase(
				std::remove_if(spaces.begin(), spaces.end(),
					[](const Space& s) {
						return s.x1 >= s.x2;
					}),
				spaces.end()
			);

			std::sort(spaces.begin(), spaces.end(),
				[](const Space& a, const Space& b) {
					if (a.pos_y != b.pos_y) return a.pos_y < b.pos_y;
					return (a.x2 - a.x1) < (b.x2 - b.x1);
				});
		}

		void addFreeSpaceOver(size_t currentShelf, const Item& item) {
			shelves[currentShelf].spaces.push_back({
				item.getPos()[0],
				item.getPos()[0] + item[0],
				item.getPos()[1] + item[1]
			});

			cleanupFreeSpace(currentShelf); // recommended over counter
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

		bool tryAndFitIntoFreeSpace(){
			packed = original;
			for (size_t i = 0; i < shelves.size(); i++){
				for (Space& space : shelves[i].spaces) {
					//get x,y extent
					size_t x = space.x2 - space.x1;
					size_t y = shelves[i].y_start + shelves[i].height - space.pos_y;

					if (original[0] <= x && original[1] <= y) {
						placeItemInFreeSpace(i, space, original);
						packed = original;
						return true;
					}
					if (rotated[0] <= x && rotated[1] <= y) {
						placeItemInFreeSpace(i, space, rotated);
						packed = rotated;
						return true;
					}
				}
			}
			return false;
		}

		bool tryAndPutAtEndOfShelves(){ 
			for (size_t i = 0; i < shelves.size(); i++){
				size_t x = shelves[i].x_limit, 
					   y = shelves[i].y_start;

				bool isShelfFlexible = (i == shelves.size()-1);

				packed = original;
				for (int j = 0; j < 2; j++){
					if (x + packed[0] <= binSize[0] && y + packed[1] <= binSize[1] && z + packed[2] <= binSize[2]){
						placeItem(x, y, i, packed);
						if (isShelfFlexible){
							shelves[i].height = std::max(packed[1], shelves[i].height); //the current shelf depth is not yet fixed, no shelves after
						}
						shelves[i].x_limit = x + packed[0];
						return true;
					}
					//rotate the item along the z axis to try to fit
					packed = rotated;
				}
			}
			return false;
		}

		bool tryNewShelfAndAdd(){
			packed = original;
			size_t lastShelfIndex = shelves.size()-1;
			if (shelves[lastShelfIndex].x_limit > 0){ //only try if the last shelf was not empty, otherwise wouldn't help to add a shelf
				size_t y = shelves[lastShelfIndex].y_start + shelves[lastShelfIndex].height;

				for (int i = 0; i < 2; i++){
					if (packed[0] <= binSize[0] && y + packed[1] <= binSize[1] && z + packed[2] <= binSize[2]){
						shelves.push_back({packed[0], y, packed[1], {}});
						placeItem(0, y, lastShelfIndex+1, packed);
						return true;
					}
					//rotate the item along the z axis to try to fit
					packed = rotated;
				}
			}
			return false;
		}
	public:
		Shelf2DPacker(Vec3 size) : binSize(size) { z = 0; reset(); }
		Shelf2DPacker(size_t z, Vec3 size) : z(z), binSize(size) { reset(); }
		Shelf2DPacker() { z = 0; reset(); }

		void setZ(size_t z){
			this->z = z;
		}

		void setBinSize(Vec3 size) { 
			binSize = size; 
		}

		size_t getLayerHeight() {
			return layerHeight;
		}
		
		void reset() {
			shelves = {};
			placed = {};
			layerHeight = 0;
		}

		bool pack(Item& item) {			
			// if (item[0] < item[1]){
			// 	std::swap(item[0], item[1]);
			// }
			original = rotated = item;
			std::swap(rotated[0], rotated[1]);
			
			//try to fit in free space
			if (tryAndFitIntoFreeSpace()){
				item = packed;
				placed.push_back(item);
				return true;
			}

			// nothing was placed yet
			if (shelves.size() == 0){
				shelves.push_back({0, 0, 0, {}});
			}
			
			//try to pack at the end of a shelf
			if (tryAndPutAtEndOfShelves()){
				item = packed;
				placed.push_back(item);
				return true;
			}

			//could'nt put it any shelf, try to open a new shelf
			if (tryNewShelfAndAdd()){
				item = packed;
				placed.push_back(item);
				return true;
			}			

			return false;
		}

		friend struct ::ShelfTester;
	};
}


#endif
