__kernel void multiply(__global int* inA, __global int* inB, __global int* out, int size)
{
	size_t id = get_global_id(0);
	if(id < size)
	{
		out[id] = inA[id] * inB[id];
	}
}

