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

#define RET_IF_FALSE(expression) \
	if (!(expression)) { \
		return false; \
	}

struct Instance {
	unsigned processors_number = 0;
	std::vector<unsigned> tasks;

	bool loadFromFile(const std::string& path) {
		std::ifstream file(path);
		unsigned tasks_number;

		RET_IF_FALSE(file.is_open());
		RET_IF_FALSE(file >> processors_number);
		RET_IF_FALSE(file >> tasks_number);

		tasks.reserve(tasks_number);
		for (unsigned i = 0; i < tasks_number; i++) {
			unsigned task_length;
			RET_IF_FALSE(file >> task_length);
			tasks.push_back(task_length);
		}

		return true;
	}

	void print() {
		std::cout << processors_number << "\n";
		std::cout << tasks.size() << "\n";
		for (const auto& e : tasks) {
			std::cout << e << "\n";
		}
	}
};

unsigned calculate_cmax(const Instance& instance, const std::vector<unsigned>& tasks_assign) {
	std::vector<unsigned> processors(instance.processors_number, 0);
	for (unsigned i = 0; i < instance.tasks.size(); i++) {
		processors[tasks_assign[i]] += instance.tasks[i];
	}

	return *std::max_element(processors.begin(), processors.end());
}

double logarithm(double value, double base) {
	return std::log(value) / std::log(base);
}

template <typename T>
void print_vec(const std::vector<T>& vec) {
	for (const auto& e : vec) {
		std::cout << e << " ";
	}
	std::cout << "\n";
}

struct Solver {
	virtual unsigned solve() = 0;
};

struct BruteForceSolverSync : public Solver {
	BruteForceSolverSync(const Instance& instance) :
		instance(instance)
	{}

	unsigned solve() override {
		std::vector<unsigned> tasks_assign(instance.tasks.size(), 0);
		unsigned pointer = 0;
		unsigned best = std::numeric_limits<unsigned>::max();

		while (true) {
			best = std::min(best, calculate_cmax(instance, tasks_assign));
			while (tasks_assign[pointer] == instance.processors_number - 1) {
				tasks_assign[pointer++] = 0;

				if (pointer == tasks_assign.size()) {
					goto solve_end;
				}
			}

			tasks_assign[pointer]++;
			pointer = 0;
		}

		solve_end:
		return best;
	}

private:
	const Instance& instance;
};

struct BruteForceSolverParalell : public Solver {
	BruteForceSolverParalell(const Instance& instance, unsigned threads_num = 4) :
		instance(instance),
		threads_num(threads_num)
	{}

	unsigned solve() override {
		unsigned best = std::numeric_limits<unsigned>::max();

		auto processors = instance.processors_number;
		auto locked_tasks = std::ceil(logarithm(threads_num, processors));
		auto total_tasks = instance.tasks.size();
		auto unlocked_tasks = total_tasks - locked_tasks;
		int jobs_number = std::pow(processors, locked_tasks);

		#pragma omp parallel for num_threads(threads_num)
		for (int job_id = 0; job_id < jobs_number; job_id++)
		{
			unsigned local_best = std::numeric_limits<unsigned>::max();
			std::vector<unsigned> tasks_assign(total_tasks, 0);
			init_tasks_assign(tasks_assign, job_id);

			unsigned pointer = 0;
			while (true) {
				local_best = std::min(local_best, calculate_cmax(instance, tasks_assign));

				while (tasks_assign[pointer] == processors - 1) {
					tasks_assign[pointer++] = 0;

					if (pointer == unlocked_tasks) {
						goto local_best_calculating_end;
					}
				}

				tasks_assign[pointer]++;
				pointer = 0;
			}

			local_best_calculating_end:
			#pragma omp critical
			best = std::min(best, local_best);
		}

		return best;
	}

	void init_tasks_assign(std::vector<unsigned>& tasks_assign, int job_id) {
		unsigned pointer = tasks_assign.size() - 1;
		for (int i = 0; i < job_id; i++) {
			while (tasks_assign[pointer] == instance.processors_number - 1 && pointer >= 0) {
				tasks_assign[pointer--] = 0;
			}

			tasks_assign[pointer]++;
			pointer = tasks_assign.size() - 1;
		}
	}

private:
	Instance instance;
	unsigned threads_num;
};

int main(int argc, char *argv[]) {
	if (argc != 4) {
		std::cerr << "Incorrect usage\n";
		std::cerr << "Use: " << argv[0] << " <instance> <threads_num> <mode>\n";
		std::cerr << "\t available modes: syn, par\n";
		std::cerr << "\t threads_num is irrelevant if mode is syn\n";
		return 1;
	}

	std::string instance_path = argv[1];
	int threads_num = std::stoi(argv[2]);
	std::string mode = argv[3];
	
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
	} else {
		std::cerr << "Unsupported mode\n";
		return 1;
	}

	auto start = std::chrono::high_resolution_clock::now();
	auto solution = solver->solve();
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff = end - start;
	double total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / std::pow(10, 9);

	std::cerr << "[RESULT] " << solution << "\n";
	std::cout << total_time << "\n";
}

