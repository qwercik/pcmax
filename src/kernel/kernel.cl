void init_tasks_assign(unsigned *tasks_mapping, size_t ptr, unsigned base, size_t id)
{
    while (id >= base)
    {
        tasks_mapping[ptr--] = id % base;
        id /= base;
    }

    tasks_mapping[ptr--] = id;

    while (ptr > 0)
    {
        tasks_mapping[ptr--] = 0;
    }

    tasks_mapping[0] = 0;
}

unsigned get_cmax(unsigned *processors_total, unsigned processors_number, __constant unsigned *tasks_durations, size_t tasks_number, unsigned *tasks_mapping)
{
    for (unsigned i = 0; i < processors_number; i++) {
        processors_total[i] = 0;
    }

    for (size_t i = 0; i < tasks_number; i++) {
        processors_total[tasks_mapping[i]] += tasks_durations[i];
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
    unsigned tasks_number,
    unsigned unlocked_tasks,
    unsigned processors_number
)
{
	size_t id = get_global_id(0);
    unsigned tasks_mapping[20];
    unsigned processors_total[8];

    init_tasks_assign(tasks_mapping, tasks_number - 1, processors_number, id);

    unsigned best = 999999999;
    size_t ptr = 0;
    while (true) {
        unsigned cmax = get_cmax(processors_total, processors_number, tasks_durations, tasks_number, tasks_mapping);
        if (cmax < best) {
            best = cmax;
        }

        while (tasks_mapping[ptr] == processors_number - 1)
        {
            tasks_mapping[ptr++] = 0;
            if (ptr == unlocked_tasks)
            {
                goto end;
            }
        }

        tasks_mapping[ptr]++;
        ptr = 0;
    }

    end:
    answers[id] = best;
}

