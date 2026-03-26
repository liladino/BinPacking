#ifndef PACKER_H
#define PACKER_H

#include "Bin.h"
#include <vector>
#include <set>

class Packer {
protected:
	std::vector<Bin3> packed;
	std::array<size_t, 3> limits;
public:
	void setLimits(size_t x, size_t y, size_t z){ limits = {x, y, z}; }

	bool intersects(const Bin3& A, const Bin3& B) const {
		for (size_t i = 0; i < 3; i++){
			if (A.getPos(i) >= B.getPos(i) + B[i] ||
				B.getPos(i) >= A.getPos(i) + A[i]) {
				return false;
			}
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

	virtual bool pack(Bin3 toPack) = 0;

    virtual ~Packer() = default;
};


#endif