#include <cstdint>

extern "C" {

__global__ void add_buffers(__device__ int32_t* a, __device__ int32_t * b , __device__ int32_t* c ) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    c[i] = a[i] + b[i];
}

}