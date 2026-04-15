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
public:
	/* Sets the limits of the container.
	 * Automatically sets the order to stand on its larget face.
	 */
	virtual void setLimits(size_t x, size_t y, size_t z){ 
		std::vector<size_t> v = {x, y, z}; 
		std::sort(v.begin(), v.end(), std::greater<size_t>());
		binSize = {v[0], v[1], v[2]}; 
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

	size_t volume() const {
		return binSize[0] * binSize[1] * binSize[2];
	}
	size_t usedVolume() const {
		size_t acc = 0;
		for (auto& x : packed) acc += x.volume(); 
		return acc;
	}

	virtual bool pack(Item& toPack) = 0;

	virtual ~Packer() = default;
};


#endif 
