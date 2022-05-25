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
#include <functional>

#include <pcmax/utils.hpp>
#include <pcmax/Instance.hpp>
#include <pcmax/solvers/Solver.hpp>
#include <pcmax/solvers/BruteForceSolverSeq.hpp>
#include <pcmax/solvers/BruteForceSolverParalell.hpp>
#include <pcmax/solvers/BruteForceSolverOcl.hpp>

int main(int argc, char *argv[]) {
    const int DEFAULT_THREADS = 4;

	if (argc != 3 && argc != 4) {
		std::cerr << "Incorrect usage\n";
		std::cerr << "Use: " << argv[0] << " <instance> <mode> [threads_num = " << DEFAULT_THREADS << "]\n";
		std::cerr << "\t available modes: seq, par, ocl\n";
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
	if (mode == "seq") {
		solver = std::make_unique<BruteForceSolverSeq>(instance);
	} else if (mode == "par") {
		solver = std::make_unique<BruteForceSolverParalell>(instance, threads_num);
	} else if (mode == "ocl") {
        solver = std::make_unique<BruteForceSolverOcl>(instance);
    } else {
		std::cerr << "Unsupported mode\n";
		return 1;
	}

    unsigned solution;
    double total_time = measureTime([&]() {
        solution = solver->solve();
    });

    std::cerr << "[Result] " << solution << "\n";
	std::cout << total_time << "\n";
}
