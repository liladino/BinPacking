#include <iostream>
#include <array>
#include <vector>
#include <utility>
#include <optional>
#include <set>

using std::array;
using std::vector;
using std::set;
using std::pair;
using std::optional;

struct Position3 {
	array<size_t, 3> coords;
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
	array<size_t, 3> extent;
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

class GreedyPack {
	struct ComparePOI {
		bool operator()(const Position3& a, const Position3& b) const {
			if (a[2] != b[2]) return a[2] < b[2]; // z
			if (a[1] != b[1]) return a[1] < b[1]; // y
			return a[0] < b[0];                   // x
		}
	};

	vector<Bin3> packed;
	set<Position3, ComparePOI> pointsOfInterest;
	array<size_t, 3> limits;
public:
	GreedyPack(){
		packed = {};
		pointsOfInterest.insert(Position3(0, 0, 0));
		limits = {1, 1, 1};
	}
	void setLimits(size_t x, size_t y, size_t z){ limits = {x, y, z}; }

	bool intersects(const Bin3& A, const Bin3& B){
		for (size_t i = 0; i < 3; i++){
			if (A.getPos(i) >= B.getPos(i) + B[i] ||
				B.getPos(i) >= A.getPos(i) + A[i]) {
				return false;
			}
		}
		return true;
	}

	bool intersectsAny(const Bin3& toPack){
		for (const auto& b : packed){
			if (intersects(b, toPack)) return true;
		}
		return false;
	}
	
	bool fitsWithinLimits(const Bin3& toPack){
		for (size_t i = 0; i < 3; i++){
			if (toPack.getPos(i) + toPack[i] > limits[i]) return false;
		}
		return true;
	}

	void updatePOI(Bin3& toPack){
		auto& pos = toPack.getPos(); 
		pointsOfInterest.insert({pos[0] + toPack[0], pos[1], pos[2]}); 
  	  	pointsOfInterest.insert({pos[0], pos[1] + toPack[1], pos[2]});
   		pointsOfInterest.insert({pos[0], pos[1], pos[2] + toPack[2]});
	}

	bool pack(Bin3 toPack) {
		//std::sort(pointsOfInterest.begin(), pointsOfInterest.end(), comparePoints);

		for (auto& p : pointsOfInterest) {
			toPack.setPos(p);
			if (!fitsWithinLimits(toPack)) continue;
			if (intersectsAny(toPack)) continue;

			packed.push_back(toPack);
			updatePOI(toPack);
			pointsOfInterest.erase(p);
			return true;
		}
		return false;
	}
};

int main(){
	GreedyPack greedy;
	greedy.setLimits(150,20,15);
	int i = 0;
	while (greedy.pack(Bin3(1, 2, 3))) {
		i++;
	}
	std::cout << i << std::endl;

	return 0;
}