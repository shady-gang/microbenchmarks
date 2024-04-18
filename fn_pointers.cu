#include <cstdint>

extern "C" {

__device__ int f(int x, int y) {
    return x + y;
}

__device__ int g(int x, int y) {
    return x * y;
}

__device__ int h(int x, int y) {
    return x - y;
}

__device__ int k(int x, int y) {
    if (y == 0)
        return x;
    return x / y;
}

__global__ void fn_ptrs(__device__ int32_t* a, __device__ int32_t* b, __device__ int32_t* c) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    int (*fns[4])(int, int) = { f, g, h, k };
    int (*fn)(int, int) = fns[i % 4];

    c[i] = fn(a[i], b[i]) + 41;
}

}