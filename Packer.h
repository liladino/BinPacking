#ifndef PACKER_H
#define PACKER_H

#include "Bin.h"
#include "IRotationPolicy.h"
#include <vector>
#include <set>
#include <memory>
#include <algorithm>

class Packer {
protected:
	std::vector<Bin3> packed;
	Vec3 limits;
	std::unique_ptr<IRotationPolicy> rotationPolicy;
public:
	/* Sets the limits of the container.
	 * Automatically sets the order to stand on its larget face.
	 */
	void setLimits(size_t x, size_t y, size_t z){ 
		std::vector<size_t> v = {x, y, z}; 
		std::sort(v.begin(), v.end(), std::greater<size_t>());
		limits = {v[0], v[1], v[2]}; 
	}
	
	void setPolicy(std::unique_ptr<IRotationPolicy> rotationPolicy) {
		this->rotationPolicy = std::move(rotationPolicy);
	}

	bool intersects(const Bin3& a, const Bin3& b) const {
		for (size_t i = 0; i < 3; ++i) {
			const bool separated =
				a.getPos(i) + a[i] <= b.getPos(i) ||
				b.getPos(i) + b[i] <= a.getPos(i);
			if (separated) return false;
		}
		return true;
	}

	bool intersectsAny(const Bin3& toPack) const {
		for (const auto& b : packed){
			if (intersects(b, toPack)) return true;
		}
		return false;
	}
	
	bool fitsWithinLimits(const Bin3& toPack) const {
		for (size_t i = 0; i < 3; i++){
			if (toPack.getPos(i) + toPack[i] > limits[i]) return false;
		}
		return true;
	}

	size_t volume() const {
		return limits[0] * limits[1] * limits[2];
	}
	size_t usedVolume() const {
		size_t acc = 0;
		for (auto& x : packed) acc += x.volume(); 
		return acc;
	}

	virtual bool pack(Bin3 toPack) = 0;

    virtual ~Packer() = default;
};


#endif 
