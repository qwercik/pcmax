#pragma once

#include <pcmax/Instance.hpp>
#include <pcmax/solvers/Solver.hpp>

struct BruteForceSolverSeq : public Solver {
	BruteForceSolverSeq(const Instance& instance) :
		instance(instance)
	{}

	unsigned solve() override {
		std::vector<unsigned> tasks_assign(instance.tasks.size(), 0);
		unsigned pointer = 0;
		unsigned best = std::numeric_limits<unsigned>::max();

		while (true) {
			best = std::min(best, instance.calculate_cmax(tasks_assign));
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

