#ifndef PACKER_H
#define PACKER_H

#include "Bin.h"
#include "IRotationPolicy.h"
#include <vector>
#include <set>

class Packer {
protected:
	std::vector<Bin3> packed;
	std::array<size_t, 3> limits;
public:
	// updates the admissible container dimensions; derived packers decide whether and how internal search structures are updated.
	void setLimits(size_t x, size_t y, size_t z){ limits = {x, y, z}; }

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