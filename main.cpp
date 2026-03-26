#include "Bin.h"
#include "GreedyPacker.h"
#include <iostream>

using std::array;
using std::vector;
using std::set;
using std::pair;

int main(){
	GreedyPacker greedy;
	greedy.setLimits(150,20,15);
	int i = 0;
	while (greedy.pack(Bin3(1, 2, 3))) {
		i++;
	}
	std::cout << i << std::endl;

	return 0;
}