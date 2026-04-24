#include "io.h"

std::string jsonData(std::string name, size_t val){
	std::stringstream ss;
	ss << "\"" << name << "\": " << val;
	return ss.str();
}

void exportPackingToJSON(Packer* packer, std::string outfile){
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

	auto items = packer->getPacked();
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
	return items.size()/3;
}