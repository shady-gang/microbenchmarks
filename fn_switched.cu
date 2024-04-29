#include <cstdint>

extern "C" {

__noinline__ __device__ int f(int x, int y) {
    return x + y;
}

__noinline__ __device__ int g(int x, int y) {
    return x * y;
}

__noinline__ __device__ int h(int x, int y) {
    return x - y;
}

__noinline__ __device__ int k(int x, int y) {
    if (y == 0)
        return x;
    return x / y;
}

__global__ void fn_ptrs(__device__ int32_t* a, __device__ int32_t* b, __device__ int32_t* c) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    switch (i % 4) {
    case 0: c[i] = f(a[i], b[i]) + 41; return;
    case 1: c[i] = g(a[i], b[i]) + 41; return;
    case 2: c[i] = h(a[i], b[i]) + 41; return;
    case 3: c[i] = k(a[i], b[i]) + 41; return;
    }
}

}