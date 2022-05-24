#pragma once
#include "../common.hpp"

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
		int pointer = tasks_assign.size() - 1;
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


