#ifndef BIN_H
#define BIN_H

#include <stdlib.h>
#include <array>
#include <utility>

struct Vec3 {
	std::array<size_t, 3> coords;
	Vec3(){ coords = {}; }
	Vec3(size_t x, size_t y, size_t z){ coords = {x, y, z};}
	
	//consts:
	size_t operator[](size_t index) const{
		return coords[index];
	}
	
	//references:
	size_t& operator[](size_t index){
		return coords[index];
	}

	bool operator<(const Vec3& other) const {
		if (coords[0] != other[0]) return coords[0] < other[0];
		if (coords[1] != other[1]) return coords[1] < other[1];
		if (coords[2] != other[2]) return coords[2] < other[2];
		return false;
	}
};

struct Bin3 {
	std::array<size_t, 3> extent;
	Vec3 position;

	Bin3(){ 
		extent = {0, 0, 0};
	}
	Bin3(size_t x, size_t y, size_t z){ 
		extent = {x, y, z};
	}

	void setPos(const Vec3& p){
		position = p;
	}

	size_t volume() const {
		return extent[0] * extent[1] * extent[2];
	}

	//consts: 
	size_t operator[](size_t index) const {
		return extent[index];
	}
	const Vec3& getPos() const {
		return position;
	}
	size_t getPos(size_t index) const {
		return position[index];
	}

	//references:
	size_t& operator[](size_t index) {
		return extent[index];
	}
	Vec3& getPos() {
		return position;
	}
	size_t& getPos(size_t index) {
		return position[index];
	}
};

#endif