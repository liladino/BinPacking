#ifndef BIN_H
#define BIN_H

#include <stdlib.h>
#include <array>
#include <utility>

struct Position3 {
	std::array<size_t, 3> coords;
	Position3(){ coords = {}; }
	Position3(size_t x, size_t y, size_t z){ coords = {x, y, z};}
	
	//consts:
	size_t operator[](size_t index) const{
		return coords[index];
	}
	
	//references:
	size_t& operator[](size_t index){
		return coords[index];
	}
};

struct Bin3 {
	std::array<size_t, 3> extent;
	Position3 position;

	Bin3(){ 
		extent = {0, 0, 0};
	}
	Bin3(size_t x, size_t y, size_t z){ 
		extent = {x, y, z};
	}

	void setPos(const Position3& p){
		position = p;
	}

	//consts: 
	size_t operator[](size_t index) const {
		return extent[index];
	}
	const Position3& getPos() const {
		return position;
	}
	size_t getPos(size_t index) const {
		return position[index];
	}

	//references:
	size_t& operator[](size_t index) {
		return extent[index];
	}
	Position3& getPos() {
		return position;
	}
	size_t& getPos(size_t index) {
		return position[index];
	}
};

#endif