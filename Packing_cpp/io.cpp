#include "io.h"

template<typename T>
std::string jsonData(const std::string& name, T val){
	std::stringstream ss;
	ss << "\"" << name << "\": " << val;
	return ss.str();
}
template<>
std::string jsonData<const std::string&>(const std::string& name, const std::string& val){
	std::stringstream ss;
	ss << "\"" << name << "\": \"" << val << "\"";
	return ss.str();
}

void exportPackingToJSON(Packer* packer, const std::string& outfile){
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
	std::ofstream data(outfile);
	if (!data.is_open()){
		std::cerr << "Couldn't open file " << outfile << std::endl;
		return;
	}
	std::stringstream ss;
	ss << "{\n\t\"bin\": { " << jsonData("w", packer->getLimits()[0]) << ", " << jsonData("h", packer->getLimits()[1]) << ", " << jsonData("d", packer->getLimits()[2]) << " },\n";
	ss << "\t\"items\": [\n";

	auto items = packer->getPackedList();
	for (int i = 0; i < items.size(); i++){
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
	std::string s = ss.str();
	data << s;
	// cout << s;
}

size_t importItems(const std::string& infile, std::vector<size_t>& items){
	std::ifstream data(infile);
	if (!data.is_open()){
		std::cerr << "Couldn't open file " << infile << std::endl;
		return 0;
	}
	size_t x;
	while (data >> x){
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
	}
	*/
	// std::ofstream data(outfile);
	// if (!data.is_open()){
	// 	std::cerr << "Couldn't open file " << outfile << std::endl;
	// 	return;
	// }

	std::stringstream ss;
	ss << "{\n\t" << jsonData("bin_needed", neededBin) << ",\n";
	ss << "\t" << jsonData("all_items", allItems) << ",\n";
	ss << "\t" << jsonData("packed", packer->getPacked()) << ",\n";
	ss << "\t\"packed_items\": [";
	std::string delim = " ";
	for (const auto& x : packer->getPackedList()){
		ss << delim << x.ID;
		delim = ", ";
	} 
	ss << " ]\n";
	// ss << "\t" << jsonData("used_volume_cm3", (double)packer->usedVolume() / 1000.0);
	ss << "}";
	
	return ss.str();
}