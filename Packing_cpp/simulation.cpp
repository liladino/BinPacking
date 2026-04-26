#include "simulation.h"

std::map<std::string, Vec3> limits = 
{
	{"test1", {10, 10, 10}},
	{"test2", {12, 10, 12}},
	{"xs", {235, 150, 120}},
	{"s",  {300, 240,  60}},
	{"m",  {500, 300, 140}},
	{"l",  {500, 300, 290}},
	{"xl", {600, 500, 320}}
};

std::vector<std::vector<std::string>> chains = 
{
	// {"test1", "test2"},
	{"xs", "m", "l", "xl"},
	{"s",  "m", "l", "xl"}
};

void simul(Packer* packer, size_t items[], size_t n, const std::string& outfile){
	// for (size_t chainIndex = 0; chainIndex < chains.size(); chainIndex++){
	size_t chainIndex = 0;
		packer->clear();

		auto limit = limits[chains[chainIndex][0]];
		packer->setLimits(limit[0], limit[1], limit[2]);

		size_t chain_j = 0, i = 0, itemID = 0, succ = 0;

		while (itemID < n) {
			// cout << "Trying to pack " << itemID+1 << "." << endl;
			Item item(items[i], items[i+1], items[i+2], itemID);

			if (packer->pack(item)) {
				itemID++;
				succ++;
				i += 3;
			}
			else {
				if (chain_j+1 < chains[chainIndex].size()){
					chain_j++;
					limit = limits[chains[chainIndex][chain_j]];
					packer->setLimits(limit[0], limit[1], limit[2]);
					// std::cout << "at " << itemID+1 << ". resized to " << chains[chainIndex][chain_j] << " bin." << std::endl;
				}
				else{
					//item couldnt be fit into biggest bin, skip
					itemID++;
					i += 3;
					// break;
				}
			}
		}

		exportPackingToJSON(packer, "../data.json");
		std::cout << metaDataToJSON(chains[chainIndex][chain_j], n, packer) << std::endl;
	// }
}

void greedy(size_t chainIndex, size_t items[], size_t n, int policy, std::string outfile){
	
}

void simulate(size_t algorithm, size_t items[], size_t n, const std::string& outfile){
	if (algorithm < 4){
		GreedyPacker greedy;
		switch (algorithm){
			case 1: greedy.setPolicy(std::make_unique<RP_largestFaceUp>()); break;
			case 2: greedy.setPolicy(std::make_unique<RP_minLeftoverSlack>()); break;
			case 3: greedy.setPolicy(std::make_unique<RP_tryFirstFitting>()); break;
			default: break;
		}
		simul(&greedy, items, n, outfile);
	}
	else if (algorithm == 4){
		ShelfPacker s;
		simul(&s, items, n, outfile);
	}
}
