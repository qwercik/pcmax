void init_tasks_assign(__local unsigned *tasks_mapping, unsigned size, size_t base, size_t id)
{
    size_t ptr = size - 1;
    while (id >= base)
    {
        tasks_mapping[ptr--] = id % base;
        id /= base;
    }

    while (ptr > 0)
    {
        tasks_mapping[ptr--] = 0;
    }

    tasks_mapping[ptr] = 0;
}

unsigned get_cmax(__local unsigned *processors_total, unsigned processors_number, __constant unsigned *tasks_durations, size_t tasks_number, __local unsigned *tasks_mapping)
{
    for (unsigned i = 0; i < processors_number; i++) {
        processors_total[i] = 0;
    }

    for (size_t i = 0; i < tasks_number; i++) {
        processors_total[i] += tasks_durations[tasks_mapping[i]];
    }

    unsigned biggest = 0;
    for (size_t i = 0; i < tasks_number; i++) {
        unsigned val = processors_total[i];
        if (val > biggest) {
            biggest = val;
        }
    }

    return biggest;
}

__kernel void find_best(
    __constant unsigned *tasks_durations,
    __global unsigned *answers,
    __local unsigned *tasks_mapping,
    unsigned unlocked_tasks,
    unsigned processors_number,
    __local unsigned *processors_total
)
{
	size_t id = get_global_id(0);
    size_t tasks_number = get_global_size(0);
    init_tasks_assign(tasks_mapping, tasks_number, processors_number, id);

    unsigned best = 999999999;
    size_t ptr = 0;
    while (true) {
        unsigned cmax = get_cmax(processors_total, processors_number, tasks_durations, tasks_number, tasks_mapping);
        if (cmax < best) {
            best = cmax;
        }

        while (tasks_mapping[pointer] == processors_number - 1)
        {
            tasks_mapping[pointer++] = 0;
            if (pointer == unlocked_tasks)
            {
                goto end;
            }
        }

        tasks_mapping[pointer]++;
        pointer = 0;
    }

    end:
    answers[id] = best;
}

