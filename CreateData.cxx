#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "json.hpp"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << "numOps" << std::endl;
		exit(EXIT_FAILURE);
	}
	unsigned int numOps = 0;
	if (sscanf(argv[1], "%d", &numOps) != 1 || numOps == 0) {
		std::cerr << "Usage: " << argv[0] << "numOps" << std::endl;
		std::cerr << "numOps must be a positive integer" << std::endl;
		exit(EXIT_FAILURE);
	}
	// C++11 random number tutorial: https://gist.github.com/PhDP/5289449
	// Seed random number generator
	std::mt19937_64 rng(time(0));
	// Create uniform distribution
	std::uniform_int_distribution<int> unif(
		std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	std::uniform_int_distribution<int> opDist(0,10);
	nlohmann::json result;
	std::vector<int> keys;		
	result["metadata"]["numOps"] = numOps;
	unsigned int totalZeros = (int) floor(log10((double) numOps)) + 1;
	for (size_t op = 1; op <= numOps; op++) {
		int operation = opDist(rng);
		int opDigits = (int) floor(log10((double) op)) + 1;
		std::string opKey = std::string(totalZeros - opDigits, '0')
			.append(std::to_string(op));
		if (operation == 0 && !keys.empty()) {
			result[opKey] = nlohmann::json();
			result[opKey]["operation"] = "Delete";
			result[opKey]["key"] = keys.back();
			keys.pop_back();
		} else if (operation == 1 && !keys.empty()) {
			result[opKey] = nlohmann::json();
			result[opKey]["operation"] = "DeleteMin";
			std::sort(keys.rbegin(), keys.rend());
			keys.pop_back();
		} else {
			int x = unif(rng);
			result[opKey] = nlohmann::json();
			result[opKey]["operation"] = "Insert";
			result[opKey]["key"] = x;
			keys.push_back(x);
		}
	}
	std::cout << result.dump(2) << std::endl;
}
