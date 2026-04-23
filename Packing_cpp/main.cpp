#include "Item.h"
#include "test.h"
#include "GreedyPacker.h"
#include "ShelfPacker.h"
#include "export.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
using namespace std;

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

void simul(Packer* packer, size_t chainIndex, size_t items[], size_t n){
	auto limit = limits[chains[chainIndex][0]];
	packer->setLimits(limit[0], limit[1], limit[2]);

	size_t chain_j = 0, i = 0, packed = 0, succ = 0;

	while (packed < n) {
		// cout << "Trying to pack " << packed+1 << "." << endl;
		Item item(items[i], items[i+1], items[i+2]);

		if (packer->pack(item)) {
			packed++;
			succ++;
			i += 3;
		}
		else {
			if (chain_j+1 < chains[chainIndex].size()){
				chain_j++;
				limit = limits[chains[chainIndex][chain_j]];
				packer->setLimits(limit[0], limit[1], limit[2]);
				cout << "at " << packed+1 << ". resized to " << chains[chainIndex][chain_j] << " bin." << endl;
			}
			else{
				// cout << "Couldn't fit " << packed+1 << ". Skipped." << endl;
				packed++;
				i += 3;
				// break;
			}
		}
	}
	exportJSON(packer);
	this_thread::sleep_for(chrono::milliseconds(1000));

	cout << "\nSuccessfully packed: " << succ << " out of " << n << '.' << endl;	
	cout << "Filled volume: " << (double)packer->usedVolume() / 1000.0 << " cm^3 out of " << (double)packer->volume() / 1000.0 << " cm^3" << endl;
	double ratio = (double)packer->usedVolume() / (double)packer->volume();
	cout << "Ratio: " << ratio*100.0 << "%    (" << ratio << ")" << endl;
}

int g_policy = 0; 
void greedy(size_t chainIndex, size_t items[], size_t n){
	GreedyPacker greedy;
	auto limit = limits[chains[chainIndex][0]];
	greedy.setLimits(limit[0], limit[1], limit[2]);

	switch (g_policy){
		case 1: greedy.setPolicy(std::make_unique<RP_largestFaceUp>()); break;
		case 2: greedy.setPolicy(std::make_unique<RP_minLeftoverSlack>()); break;
		case 3: greedy.setPolicy(std::make_unique<RP_tryFirstFitting>()); break;
		default: break;
	}

	simul(&greedy, chainIndex, items, n);
}


void iterativeSimulation(size_t chainIndex, size_t items[], size_t n){

}

bool vecComp2(const Vec3& a, const Vec3& b) {
	if (a[2] != b[2]) return a[2] < b[2]; // z
	if (a[1] != b[1]) return a[1] < b[1]; // y
	return a[0] < b[0];                   // x
}

bool vecComp(const Vec3& a, const Vec3& b) {
	return a[0] * a[1] * a[2] < b[0] * b[1] * b[2];
}

int main(){
	srand(time(NULL));  
	// srand(8);
	vector<Vec3> itemVec;
	vector<size_t> items;
	for (int i = 1; i <= 100; i++){
		itemVec.push_back({(size_t)(rand() % 120 + 50), (size_t)(rand() % 120 + 50), (size_t)(rand() % 120 + 50)});
	}
	sort(itemVec.rbegin(), itemVec.rend(), vecComp);
	for (auto& x : itemVec){
		items.push_back(x[0]);
		items.push_back(x[1]);
		items.push_back(x[2]);
	}


	cout << "----------------------------------------------\n";
	for (g_policy = 0; g_policy < 4; g_policy++){
		cout << "\nPolicy: " << g_policy << endl;
		greedy(0, items.data(), items.size()/3);
	}

	cout << "----------------------------------------------\n";
	ShelfPacker s;
	simul(&s, 0, items.data(), items.size()/3);


	return 0;
}