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
// cd Packing_cpp/; make all; cd ..
// ./Packing_cpp/packer.exe -i "data/items00001.txt" -o "results/output.json"
int main(int argc, char *argv[]) {
	string inputFilePath = "", outputFilePath = "", visual = "";
	int alg = 0;
	bool firstFit = false, argVisual = false;

	ShipPolicy shipPolicy = ShipPolicy::SkipIfDoesntFit;
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
			argVisual = true;
		}
		else if (0 == strcmp("--firstFit", argv[i])){
			lastComm = Command::Other;
			firstFit = true;
		}
		else if (0 == strcmp("--shipEverything", argv[i])){
			lastComm = Command::Other;
			shipPolicy = ShipPolicy::ShipEverything;
		}
		else if (0 == strcmp("--skipIfDoesntFit", argv[i])){
			lastComm = Command::Other;
			shipPolicy = ShipPolicy::SkipIfDoesntFit;
		}
		else if (0 == strcmp("--stopIfDoesntFit", argv[i])){
			lastComm = Command::Other;
			shipPolicy = ShipPolicy::StopIfDoesntFit;
		}
		else {
			if (Command::Input == lastComm) {
				inputFilePath = argv[i];
			}
			else if (Command::Output == lastComm) {
				outputFilePath = argv[i];
			} 
			else if (Command::Algo == lastComm) {
				alg = stoi(argv[i]);
			}
			else if (Command::Visual == lastComm) {
				visual = argv[i];
			}
			lastComm = Command::Other;
		}
	}
	//inputs
	std::ifstream inFile;
    std::istream* inputStream = nullptr;
	if (inputFilePath == ""){
		inputStream = &std::cin;
	}
	else {
		inFile.open(inputFilePath);
		if (!inFile.is_open()){
			std::cerr << "Couldn't open file " << inputFilePath << std::endl;
			return 1;
		}
		inputStream = &inFile;
	}

	// output
	std::ofstream outFile;
    std::ostream* outputStream = nullptr;
	if (outputFilePath == ""){
		outputStream = &std::cout;
	}
	else {
		outFile.open(outputFilePath);
		if (!outFile.is_open()){
			std::cerr << "Couldn't open file " << outputFilePath << std::endl;
			return 1;
		}
		outputStream = &outFile;
	}

	if ("" == visual && argVisual){
		//visuals requested, but no file specified
		visual = "data.json";
	}

	std::vector<size_t> itemsVec;
	size_t items = importItems(inputStream, itemsVec); 
	if (inputStream == &inFile){
		inFile.close();
	}
	if (items == 0){
		std::cerr << "No items" << std::endl;
		return 1;
	}

	simulate(alg, itemsVec.data(), items, outputStream, firstFit, shipPolicy, visual);
	if (outputStream == &outFile){
		outFile.close();
	}

	return 0;
}