#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#define RET_IF_FALSE(expression) \
	if (!(expression)) { \
		return false; \
	}

struct Instance {
	unsigned processors_number = 0;
	std::vector<unsigned> tasks;

    bool loadFromFile(const std::string& path) {
        std::ifstream file(path);
        RET_IF_FALSE(file.is_open());
        return loadFromStream(file);
    }

	bool loadFromStream(std::istream& stream) {
		unsigned tasks_number;

		RET_IF_FALSE(stream >> processors_number);
		RET_IF_FALSE(stream >> tasks_number);

		tasks.reserve(tasks_number);
		for (unsigned i = 0; i < tasks_number; i++) {
			unsigned task_length;
			RET_IF_FALSE(stream >> task_length);
			tasks.push_back(task_length);
		}

		return true;
	}

	void print() const {
		std::cout << processors_number << "\n";
		std::cout << tasks.size() << "\n";
		for (const auto& e : tasks) {
			std::cout << e << "\n";
		}
	}

    unsigned calculate_cmax(const std::vector<unsigned>& tasks_assign) const {
        std::vector<unsigned> processors(processors_number, 0);
        for (unsigned i = 0; i < tasks.size(); i++) {
            processors[tasks_assign[i]] += tasks[i];
        }

        return *std::max_element(processors.begin(), processors.end());
    }
};

