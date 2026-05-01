#include "simulation.h"
#include <cstring>

using namespace std;

enum class Command{
	Input,
	Output,
	Algo,
	Visual,
	Other
};


// g++ main.cpp simulation.cpp io.cpp -Wall
int main(int argc, char *argv[]) {
	if (argc < 2){
		std::cerr << "No input specified" << std::endl;
		return 0;
	}

	string infile = "", outfile = "output.json", visual = "";
	int alg = 0;
	Command lastComm = Command::Other; 
	for (int i = 1; i < argc; i++){
		if (0 == strcmp("--input", argv[i]) || 0 == strcmp("-i", argv[i])){
			lastComm = Command::Input;
		}
		else if (0 == strcmp("--output", argv[i]) || 0 == strcmp("-o", argv[i])){
			lastComm = Command::Output;
		}
		else if (0 == strcmp("--algorithm", argv[i]) || 0 == strcmp("-a", argv[i])){
			lastComm = Command::Algo;
		}
		else if (0 == strcmp("--visual", argv[i]) || 0 == strcmp("-v", argv[i])){
			lastComm = Command::Visual;
		}
		else {
			if (Command::Input == lastComm) {
				infile = argv[i];
			}
			else if (Command::Output == lastComm) {
				outfile = argv[i];
			} 
			else if (Command::Algo == lastComm) {
				alg = stoi(argv[i]);
			}
			else if (Command::Visual == lastComm) {
				visual = stoi(argv[i]);
			}
			lastComm = Command::Other;
		}
	}
	if (infile == ""){
		std::cerr << "No input specified" << std::endl;
		return 0;
	}

	std::vector<size_t> itemsVec;
	size_t items = importItems(infile, itemsVec); 
	std::cerr << "imp\n";
	if (items == 0){
		std::cerr << "No items" << std::endl;
		return 0;
	}

	if ("" != visual){
		simulate(alg, itemsVec.data(), items, outfile);
	}
	else {
		simulate(alg, itemsVec.data(), items, outfile);
	}
	
	return 0;
}