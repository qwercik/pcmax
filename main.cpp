#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <algorithm>
#include <omp.h>

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

struct Solver {
	virtual unsigned solve() = 0;
};

unsigned calculate_cmax(const Instance& instance, const std::vector<unsigned>& tasks_assign) {
	std::vector<unsigned> processors(instance.processors_number, 0);
	for (unsigned i = 0; i < instance.tasks.size(); i++) {
		processors[tasks_assign[i]] += instance.tasks[i];
	}

	return *std::max_element(processors.begin(), processors.end());
}

template <typename T>
void print_vec(const std::vector<T>& vec) {
	for (const auto& e : vec) {
		std::cout << e << " ";
	}
	std::cout << "\n";
}

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
			while (tasks_assign[pointer] == instance.processors_number - 1 && pointer < tasks_assign.size()) {
				tasks_assign[pointer++] = 0;
			}

			if (pointer == tasks_assign.size()) {
				break;
			}

			tasks_assign[pointer]++;
			pointer = 0;
		}
		
		return best;
	}

private:
	const Instance& instance;
};

struct BruteForceSolverParalell : public Solver {
	// Threads num have to be a power of 2
	BruteForceSolverParalell(const Instance& instance, unsigned threads_num = 4) :
		instance(instance),
		threads_num(threads_num)
	{}

	unsigned solve() override {
		unsigned best = std::numeric_limits<unsigned>::max();
		#pragma omp parallel num_threads(threads_num)
		{
			int thread_id = omp_get_thread_num();

			unsigned local_best = std::numeric_limits<unsigned>::max();
			std::vector<unsigned> tasks_assign(instance.tasks.size(), 0);
			init_tasks_assign(tasks_assign, thread_id);
			
			#pragma omp critical
			{
				std::cout << "[" << thread_id << "] ";
				print_vec(tasks_assign);
			}
			
			unsigned pointer = 0;
		}

		return best;
	}

	void init_tasks_assign(std::vector<unsigned>& tasks_assign, int thread_id) {
		unsigned pointer = tasks_assign.size() - 1;
		for (int i = 0; i < thread_id; i++) {
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
	if (argc != 2) {
		std::cerr << "Incorrect usage\n";
		std::cerr << "Use: " << argv[0] << " <instance>\n";
		return 1;
	}

	std::string instance_path = argv[1];
	
	Instance instance;
	if (!instance.loadFromFile(instance_path)) {
		std::cerr << "File doesn\'t exist\n";
		return 1;
	}

	BruteForceSolverParalell solver(instance, 8);
	std::cout << solver.solve() << "\n";
}

