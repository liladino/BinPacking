#include "Item.h"
#include "GreedyPacker.h"
#include <iostream>
#include <string>
#include <map>

using std::array;
using std::string;
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::cout;
using std::endl;

map<string, Vec3> limits = 
{
	{"test1", {10, 10, 10}},
	{"test2", {12, 10, 12}},
	{"xs", {235, 150, 120}},
	{"s",  {300, 240,  60}},
	{"m",  {500, 300, 140}},
	{"l",  {500, 300, 290}},
	{"xl", {600, 500, 320}}
};

vector<vector<string>> chains = 
{
	// {"test1", "test2"},
	{"xs", "m", "l", "xl"},
	{"s",  "m", "l", "xl"}
};

int g_policy = 0; 

void iterativeSimulation(size_t chainIndex, size_t items[], size_t n){
	GreedyPacker greedy;
	auto& limit = limits[chains[chainIndex][0]];
	greedy.setLimits(limit[0], limit[1], limit[2]);

	switch (g_policy){
		case 1: greedy.setPolicy(std::make_unique<RP_largestFaceUp>()); break;
		case 2: greedy.setPolicy(std::make_unique<RP_minLeftoverSlack>()); break;
		case 3: greedy.setPolicy(std::make_unique<RP_tryFirstFitting>()); break;
		default: break;
	}

	size_t chain_j = 0, i = 0, packed = 0;

	while (packed < n) {
		// cout << "Trying to pack " << packed+1 << "." << endl;
		if (greedy.pack(Item(items[i], items[i+1], items[i+2]))) {
			packed++;
			i += 3;
		}
		else {
			if (chain_j+1 < chains[chainIndex].size()){
				chain_j++;
				limit = limits[chains[chainIndex][chain_j]];
				greedy.setLimits(limit[0], limit[1], limit[2]);
				cout << "at " << packed+1 << ". resized to " << chains[chainIndex][chain_j] << " bin." << endl;
			}
			else{
				cout << "Couldn't fit " << packed+1 << "." << endl;
				break;
			}
		}
	}

	cout << "\nSuccessfully packed: " << packed << " out of " << n << '.' << endl;	
	cout << "Filled volume: " << (double)greedy.usedVolume() / 1000.0 << " cm^3 out of " << (double)greedy.volume() / 1000.0 << " cm^3" << endl;
	double ratio = (double)greedy.usedVolume() / (double)greedy.volume();
	cout << "Ratio: " << ratio*100.0 << "%    (" << ratio << ")" << endl;
}

int main(){
	//srand(time(NULL));
	srand(8);
	vector<size_t> items;
	for (int i = 1; i <= 100*3; i++){
		items.push_back(rand() % 120 + 50);
	}

	cout << "----------------------------------------------\n";
	for (g_policy = 0; g_policy < 4; g_policy++){
		cout << "\nPolicy: " << g_policy << endl;
		iterativeSimulation(0, items.data(), items.size()/3);
	}

	return 0;
}