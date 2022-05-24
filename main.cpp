#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <omp.h>
#include <chrono>

#include "common.hpp"
#include "modes/syn.hpp"
#include "modes/par.hpp"
#include "modes/ocl.hpp"

int main(int argc, char *argv[]) {
    const int DEFAULT_THREADS = 4;

	if (argc != 3 && argc != 4) {
		std::cerr << "Incorrect usage\n";
		std::cerr << "Use: " << argv[0] << " <instance> <mode> [threads_num = " << DEFAULT_THREADS << "]\n";
		std::cerr << "\t available modes: syn, par, ocl\n";
		std::cerr << "\t threads_num is irrelevant if mode is not par\n";
		return 1;
	}

	std::string instance_path = argv[1];
	std::string mode = argv[2];
	int threads_num = argc < 4 ? DEFAULT_THREADS : std::stoi(argv[3]);

	Instance instance;
	if (!instance.loadFromFile(instance_path)) {
		std::cerr << "File doesn\'t exist\n";
		return 1;
	}

	std::unique_ptr<Solver> solver;
	if (mode == "syn") {
		solver = std::make_unique<BruteForceSolverSync>(instance);
	} else if (mode == "par") {
		solver = std::make_unique<BruteForceSolverParalell>(instance, threads_num);
	} else if (mode == "ocl") {
        solver = std::make_unique<BruteForceSolverOcl>(instance);
    } else {
		std::cerr << "Unsupported mode\n";
		return 1;
	}

	auto start = std::chrono::high_resolution_clock::now();
	auto solution = solver->solve();
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff = end - start;
	double total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / std::pow(10, 9);

	std::cerr << "[Result] " << solution << "\n";
	std::cout << total_time << "\n";
}

