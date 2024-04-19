#include <cstdint>

#define FN_PREFIX __device__
#include "binary_tree.h"

extern "C" {

using Node = TreeNode<int>;

__global__ void treesearch(__device__ int32_t* a, __device__ int32_t * b , __device__ Node* root ) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    b[i] = !!find((Node*) root, a[i]);
}

}
