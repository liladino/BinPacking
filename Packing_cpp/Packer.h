#ifndef PACKER_H
#define PACKER_H

#include "Item.h"
#include "IRotationPolicy.h"
#include <vector>
#include <set>
#include <memory>
#include <algorithm>

class Packer {
protected:
	std::vector<Item> packed;
	Vec3 binSize;
private:
	/* Calculates the maximum extent of the items in the bin,
	* assuming that the packing started from the 0, 0, 0 corner
	*/
	std::array<size_t, 3> farthestXYZ = {0, 0, 0};
	void calculateItemExtent(){
		farthestXYZ = {0, 0, 0};
		for (const auto& item : packed){
			for (size_t i = 0; i < 3; i++){
				farthestXYZ[i] = std::max(item.getPos(i) + item[i], farthestXYZ[i]);
			}
		}
	}
public:
	/* Sets the limits of the container.
	 * Automatically sets the order to stand on its larget face.
	 */
	virtual void setLimits(size_t x, size_t y, size_t z){ 
		// std::cout << "Old: " << binSize[0] << ' ' << binSize[1] << ' ' << binSize[2] << '\n';
		std::vector<size_t> v = {x, y, z}; 
		std::sort(v.begin(), v.end(), std::greater<size_t>());
		binSize = {v[0], v[1], v[2]}; 
		// std::cout << "New: " << v[0] << ' ' << v[1] << ' ' << v[2] << std::endl;
	}

	virtual void clear(){
		packed = {};
	}
	
	Vec3 getLimits() {
		return binSize;
	}

	std::vector<Item>& getPackedList(){
		return packed;
	}

	size_t getPacked(){
		return packed.size();
	}

	size_t volume() const {
		return binSize[0] * binSize[1] * binSize[2];
	}
	size_t usedVolume() const {
		size_t acc = 0;
		for (auto& x : packed) acc += x.volume(); 
		return acc;
	}

	size_t getMinLeftoverSlack(bool recalculate = true){
		if (recalculate) calculateItemExtent();
		return std::min({
			binSize[0] - farthestXYZ[0], 
			binSize[1] - farthestXYZ[1], 
			binSize[2] - farthestXYZ[2]
		});
	}

	size_t getMaxLeftoverSlack(bool recalculate = true){
    	if (recalculate) calculateItemExtent();
		return std::max({
			binSize[0] - farthestXYZ[0], 
			binSize[1] - farthestXYZ[1], 
			binSize[2] - farthestXYZ[2]
		});
	}

	size_t getSumLeftoverSlack(bool recalculate = true){
		if (recalculate) calculateItemExtent();
		return 
			binSize[0] - farthestXYZ[0] +
			binSize[1] - farthestXYZ[1] +
			binSize[2] - farthestXYZ[2];
	}

	double getBoundingBoxVolumeRatio(bool recalculate = true){
		if (packed.size() == 0) return 0;
		if (recalculate) calculateItemExtent();
		return usedVolume() / (double)farthestXYZ[0] / (double)farthestXYZ[1] / (double)farthestXYZ[2];
	}

	bool intersects(const Item& a, const Item& b) const {
		for (size_t i = 0; i < 3; ++i) {
			const bool separated =
				a.getPos(i) + a[i] <= b.getPos(i) ||
				b.getPos(i) + b[i] <= a.getPos(i);
			if (separated) return false;
		}
		return true;
	}

	bool intersectsAny(const Item& toPack) const {
		for (const auto& b : packed){
			if (intersects(b, toPack)) return true;
		}
		return false;
	}
	
	bool fitsWithinLimits(const Item& toPack) const {
		for (size_t i = 0; i < 3; i++){
			if (toPack.getPos(i) + toPack[i] > binSize[i]) return false;
		}
		return true;
	}

	virtual bool pack(Item& toPack) = 0;
	virtual ~Packer() = default;
};


#endif 
