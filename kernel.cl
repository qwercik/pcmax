__kernel void multiply(__global int* inA, __global int* inB, __global int* outC, unsigned long int size)
{
	size_t id = get_global_id(0);
    outC[id] = inA[id] * inB[id];
}
