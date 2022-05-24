#pragma once

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

