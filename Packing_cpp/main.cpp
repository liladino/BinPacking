#include "simulation.h"
#include <cstring>

using namespace std;

enum class Command{
	Input,
	Output,
	Algo,
	Other
};

int main(int argc, char *argv[]) {
	if (argc < 2){
		return 0;
	}

	string infile = "", outfile = "data.json";
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
			lastComm = Command::Output;
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
			lastComm = Command::Other;
		}
	}

	// switch (alg){
	// 	case 4: 
	// 		{
	// 			ShelfPacker s;
	// 			simul(&s, 0, items.data(), items.size()/3, "../data.json");
	// 		}
	// 		break;
	// 	default:
	// 		greedy(0, items.data(), items.size()/3, policy, "../data.json");
	// 		break;
	// }

	return 0;
}