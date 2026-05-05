#include "simulation.h"

std::map<std::string, Vec3> limits = 
{
	// {"test1", {10, 10, 10}},
	// {"test2", {12, 10, 12}},
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


template <typename T1, typename T2, typename T3> 
struct trio{
	T1 first; T2 second; T3 third;
};

void incrementalAlgo(Packer* packer, size_t items[], size_t n, const std::string& outfile){
	std::vector<trio<size_t, size_t, std::string>> results;
	for (size_t chainIndex = 0; chainIndex < chains.size(); chainIndex++){
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

		// exportPackingToJSON(packer, "../data.json");
		// exportPackingToJSON(packer, visualFileName + std::to_string(chainIndex) + visualFileType);

		// char c; std::noskipws(std::cin); std::cin >> c;

		auto meta = metaDataToJSON(chains[chainIndex][chain_j], n, packer);
		// std::cout << meta << std::endl;
		results.push_back({packer->getPacked(), chain_j, meta});
	}

	int maxi = 0;
	for (size_t i = 1; i < chains.size(); i++){
		if (results[i].first > results[maxi].first){
			// packed more?
			maxi = i;
		}
		else if (results[i].first == results[maxi].first){
			// used a smaller bin?
			if (results[i].second < results[maxi].second){
				maxi = i;
			}
		}
	}
	
	writeMetaData(outfile, results[maxi].third);
}

//finds the minimal bin needed for given input without incremental algorithm
void firstFitAlgo(Packer* packer, size_t items[], size_t n, const std::string& outfile){
	std::vector<std::pair<std::string, Vec3>> limitsVector;
	std::for_each(limits.begin(), limits.end(), 
		[&](auto x){ 
			limitsVector.push_back(x);
		});
	std::sort(limitsVector.begin(), limitsVector.end(), 
		[](const auto& l, const auto& r){ 
			return l.second[0] * l.second[1] * l.second[2] < r.second[0] * r.second[1] * r.second[2];
		});

	for (auto x : limitsVector){ 
			std::cout << x.first << ' ';
		}
	
	std::vector<trio<size_t, size_t, std::string>> results;

	for (const auto& [limitName, currentLimits] : limitsVector) {
		packer->clear();
		packer->setLimits(currentLimits[0], currentLimits[1], currentLimits[2]);

		size_t i = 0, itemID = 0;
		bool success = true;
		while (itemID < n) {
			Item item(items[i], items[i+1], items[i+2], itemID);

			if (packer->pack(item)) {
				itemID++;
				i += 3;
			}
			else {
				success = false;
				itemID++;
				i += 3;
			}
		}

		if (success){
			auto meta = metaDataToJSON(limitName, n, packer);
			writeMetaData(outfile, meta);
			return;
		}
	}

	//couldn't fit all
	auto meta = metaDataToJSON(limitsVector[limitsVector.size()-1].first, n, packer);
	writeMetaData(outfile, meta);
}

void simulate(size_t algorithm, size_t items[], size_t n, const std::string& outfile, bool firstFit){
	if (algorithm < 4){
		GreedyPacker greedy;
		switch (algorithm){
			case 1: greedy.setPolicy(std::make_unique<RP_largestFaceUp>()); break;
			case 2: greedy.setPolicy(std::make_unique<RP_minLeftoverSlack>()); break;
			case 3: greedy.setPolicy(std::make_unique<RP_tryFirstFitting>()); break;
			default: break;
		}

		if (firstFit){
			firstFitAlgo(&greedy, items, n, outfile);
		}
		else {
			incrementalAlgo(&greedy, items, n, outfile);
		}

	}
	else if (algorithm == 4){
		ShelfPacker s;
		if (firstFit){
			firstFitAlgo(&s, items, n, outfile);
		}
		else {
			incrementalAlgo(&s, items, n, outfile);
		}
	}
}
