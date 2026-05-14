#include "io.h"

template<typename T>
std::string jsonData(const std::string& name, T val){
	std::stringstream ss;
	ss << "\"" << name << "\": " << val;
	return ss.str();
}
template<>
std::string jsonData<std::string>(const std::string& name, std::string val){
	std::stringstream ss;
	ss << "\"" << name << "\": \"" << val << "\"";
	return ss.str();
}

std::string exportPacking(Packer* packer){
	/* Format:
	{
		"bin": { "w": 15, "h": 9, "d": 8 },
		"items": [
			{ "x": 0, "y": 0, "z": 0, "w": 4, "h": 4, "d": 4 },
			{ "x": 4, "y": 0, "z": 0, "w": 3, "h": 5, "d": 2 },
			{ "x": 0, "y": 4, "z": 0, "w": 2, "h": 3, "d": 6 }
		]
	}
	*/
	std::stringstream ss;
	ss << "{\n\t\"bin\": { " << jsonData("w", packer->getLimits()[0]) << ", " << jsonData("h", packer->getLimits()[1]) << ", " << jsonData("d", packer->getLimits()[2]) << " },\n";
	ss << "\t\"items\": [\n";

	auto items = packer->getPackedList();
	for (size_t i = 0; i < items.size(); i++){
		auto& x = items[i];
		ss << "\t\t{ " << jsonData("x", x.getPos()[0]) << ", " << jsonData("y", x.getPos()[1]) << ", " << jsonData("z", x.getPos()[2]) << ", " << jsonData("w", x[0]) << ", " << jsonData("h", x[1]) << ", " << jsonData("d", x[2]); 
		if (i < items.size()-1){
			ss << " },\n";
		}
		else{
			ss << " }\n";
		}
	}

	ss << "\t]\n}";
	return ss.str();
}

void exportPackingToJSON(Packer* packer, const std::string& outfile){
	std::ofstream data(outfile);
	if (!data.is_open()){
		std::cerr << "Couldn't open file " << outfile << std::endl;
		return;
	}

	data << exportPacking(packer);
	data.close();
}

void exportPackingToJSON(const std::string& JSON, const std::string& outfile){
	std::ofstream data(outfile);
	if (!data.is_open()){
		std::cerr << "Couldn't open file " << outfile << std::endl;
		return;
	}
	data << JSON;
	data.close();
}

size_t importItems(std::istream* is, std::vector<size_t>& items){
	size_t x;
	while (*is >> x){
		items.push_back(x);
	}

	while (items.size() % 3 != 0) {
		items.pop_back();
	}

	// for (auto x : items){
	// 	std::cout << x << " ";
	// }
	// std::cout << std::endl;

	return items.size()/3;
}

std::string metaDataToJSON(const std::string& neededBin, size_t allItems, Packer* packer){
	/* Format:
	{
		"bin_needed": "xl",
		"all_items": 5,
		"packed": 4,
		"packed_items": [0, 1, 3, 4]
		"min_leftover_slack": 1,
		"sum_leftover_slack": 1,
		"max_leftover_slack": 1,
		"bounding_box_volume_ratio": 0.8
	}
	*/
	std::stringstream ss;
	ss << "\t{\n\t\t" << jsonData("bin_needed", neededBin) << ",\n";
	ss << "\t\t" << jsonData("all_items", allItems) << ",\n";
	ss << "\t\t" << jsonData("packed", packer->getPacked()) << ",\n";
	
	ss << "\t\t\"packed_items\": [";
	std::string delim = " ";
	for (const auto& x : packer->getPackedList()){
		ss << delim << x.ID;
		delim = ", ";
	} 
	ss << " ],\n";

	ss << "\t\t" << jsonData("min_leftover_slack", packer->getMinLeftoverSlack()) << ",\n";
	ss << "\t\t" << jsonData("sum_leftover_slack", packer->getSumLeftoverSlack(false)) << ",\n";
	ss << "\t\t" << jsonData("max_leftover_slack", packer->getMaxLeftoverSlack(false)) << ",\n";
	ss << "\t\t" << jsonData("bounding_box_volume_ratio", packer->getBoundingBoxVolumeRatio(false)) << "\n";
	ss << "\t}";
	
	return ss.str();
}

void writeMetaData(std::ostream* outfile, const std::vector<std::string>& data){
	if (data.size() == 0) return;

	std::string x = "[\n";
	for (const auto& str : data){
		*outfile << x << str;
		x = ",\n";
	}
	*outfile << "\n]";	
}