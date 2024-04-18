#include <cstdint>

extern "C" {

__device__ int fib(int n) {
    if (n <= 1)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

__global__ void fib_main(__device__ int32_t* a) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    a[i] = fib(i % 16);
}

}