__kernel void multiply(__global int* outC)
{
	size_t id = get_global_id(0);
    size_t gs = get_global_size(0);

    outC[id] = gs;
}

