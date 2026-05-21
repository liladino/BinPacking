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
	// {"EUR6", {800, 600, 2200}},
	// {"EUR1", {1200, 800, 2200}},
	// {"EUR2", {1200, 1200, 2200}}
};

std::vector<std::vector<std::string>> chains = 
{
	// {"test1", "test2"},
	{"xs", "m", "l", "xl"},
	{"s",  "m", "l", "xl"}
	// {"EUR6","EUR1","EUR2"}
};

template <typename T1, typename T2, typename T3> 
struct trio{
	T1 first; T2 second; T3 third;
};

size_t packWhileItCan(Packer* packer, size_t items[], const size_t n, size_t _itemID){
	while (_itemID < n) {
		size_t i = _itemID * 3;
		Item item(items[i], items[i+1], items[i+2], _itemID);

		if (packer->pack(item)) {
			_itemID++;
		}
		else {
			return _itemID;
		}
	}
	return _itemID;
}

std::pair<size_t, std::string> incrementalAlgo(Packer* packer, size_t items[], const size_t n, const size_t itemID, const ShipPolicy shipPolicy, const std::string& visualExp = ""){
	std::vector<trio<size_t, size_t, std::string>> results;
	std::vector<std::string> visuals;
	for (size_t chainIndex = 0; chainIndex < chains.size(); chainIndex++){
		packer->clear();

		auto limit = limits[chains[chainIndex][0]];
		packer->setLimits(limit[0], limit[1], limit[2]);

		size_t chain_j = 0;
		size_t _itemID = itemID;
		_itemID = packWhileItCan(packer, items, n, _itemID);
		while (_itemID < n){
			//try a bigger bin
			if (chain_j+1 < chains[chainIndex].size()){
				chain_j++;
				limit = limits[chains[chainIndex][chain_j]];
				packer->setLimits(limit[0], limit[1], limit[2]);
			}
			else{
				//no bigger bin left
				if (ShipPolicy::SkipIfDoesntFit == shipPolicy){
					_itemID++;
				}
				else if (ShipPolicy::StopIfDoesntFit == shipPolicy){
					break;
				}
				else if (ShipPolicy::ShipEverything == shipPolicy){
					break;
				}
			}
			_itemID = packWhileItCan(packer, items, n, _itemID);
		}

		if ("" != visualExp){			
			visuals.push_back(exportPacking(packer));
		}

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
	
	if ("" != visualExp) {
		exportPackingToJSON(visuals[maxi], visualExp);
		if (ShipPolicy::ShipEverything == shipPolicy && results[maxi].first < n) {
			char c; std::noskipws(std::cin); std::cin >> c;
		}
	}
	return {results[maxi].first, results[maxi].third};
}

//finds the minimal bin needed for given input without incremental algorithm
std::pair<size_t, std::string> firstFitAlgo(Packer* packer, size_t items[], const size_t n, const size_t itemID, const ShipPolicy shipPolicy, const std::string& visualExp = ""){
	std::vector<std::pair<std::string, Vec3>> limitsVector;
	std::for_each(limits.begin(), limits.end(), 
		[&](auto x){ 
			limitsVector.push_back(x);
		});
	std::sort(limitsVector.begin(), limitsVector.end(), 
		[](const auto& l, const auto& r){ 
			return l.second[0] * l.second[1] * l.second[2] < r.second[0] * r.second[1] * r.second[2];
		});
	
	std::vector<trio<size_t, size_t, std::string>> results;

	for (const auto& [limitName, currentLimits] : limitsVector) {
		packer->clear();
		packer->setLimits(currentLimits[0], currentLimits[1], currentLimits[2]);
		size_t _itemID = itemID;
		
		_itemID = packWhileItCan(packer, items, n, _itemID);
		while (_itemID < n){
			if (ShipPolicy::SkipIfDoesntFit == shipPolicy){
				if (limitName == limitsVector[limitsVector.size()-1].first){
					//only if xl size, skip
					_itemID++;
				}
				else {
					break;
				}
			}
			else if (ShipPolicy::StopIfDoesntFit == shipPolicy){
				break;
			}
			else if (ShipPolicy::ShipEverything == shipPolicy){
				break;
			}
			_itemID = packWhileItCan(packer, items, n, _itemID);
		}
		
		if (_itemID == n){
			auto meta = metaDataToJSON(limitName, n, packer);
			if ("" != visualExp){
				exportPackingToJSON(packer, visualExp);
				// char c; std::noskipws(std::cin); std::cin >> c;
			}
			return {packer->getPacked(), meta};
		}
	}

	//couldn't fit all
	auto meta = metaDataToJSON(limitsVector[limitsVector.size()-1].first, n, packer);
	if ("" != visualExp){
		exportPackingToJSON(packer, visualExp);
		if (ShipPolicy::ShipEverything == shipPolicy) {
			char c; std::noskipws(std::cin); std::cin >> c;
		}
	}
	return {packer->getPacked(), meta};
}

void simulate(Algorithm algorithm, size_t items[], const size_t n, std::ostream* outfile, bool firstFit, const ShipPolicy shipPolicy, const std::string& visualExp){
	Packer* packer = nullptr;
	GreedyPacker greedy;
	ShelfPacker shelf;
	ExtremePointsPacker epp;
	
	switch (algorithm){
		case Algorithm::GreedyNoRotation: packer = &greedy; break;
		case Algorithm::GreedyLargestFaceUp: greedy.setPolicy(std::make_unique<RP_largestFaceUp>()); packer = &greedy; break;
		case Algorithm::GreedyMinSumLeftoverSlack: greedy.setPolicy(std::make_unique<RP_minSumLeftoverSlack>()); packer = &greedy; break;
		case Algorithm::GreedyTryFirstFitting:  greedy.setPolicy(std::make_unique<RP_tryFirstFitting>()); packer = &greedy; break;
		case Algorithm::ShelfLayer: packer = &shelf; break;
		case Algorithm::ExtremePoints: packer = &epp; break;
	}

	size_t itemID = 0, lastItemID = 0;
	std::vector<std::string> jsonArray;
	while (itemID < n){
		if (firstFit){
			auto [x, str] = firstFitAlgo(packer, items, n, itemID, shipPolicy, visualExp);
			jsonArray.push_back(str);
			itemID += x;
		}
		else{
			auto [x, str] = incrementalAlgo(packer, items, n, itemID, shipPolicy, visualExp);
			jsonArray.push_back(str);
			itemID += x;
		}
		if (ShipPolicy::ShipEverything != shipPolicy){
			break;
		}
		if (lastItemID == itemID){
			//couldn't fit the current item into any bin
			break; 
		}
		lastItemID = itemID;
	}

	writeMetaData(outfile, jsonArray);
}
