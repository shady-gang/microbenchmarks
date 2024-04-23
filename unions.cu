//#include <shady.h>
#include <stdint.h>

//using namespace vcc;

#define FN_PREFIX
#include "unions.h"

#define w 256

extern "C" {

/*void debug_printff(const char*, float) __asm__("shady::prim_op::debug_printf::i");
void debug_printfi(const char*, int) __asm__("shady::prim_op::debug_printf::i");*/

__device__
S patch(S s, int o) {
  if (s.tag) {
    s.f += (float) o;
    //debug_printff("f%f", s.f);
  } else {
    s.i += o;
    //debug_printfi("i%d", s.i);
  }
  return s;
}

__device__
unsigned int hash_fnv(char* str, unsigned int length) {
    const unsigned int fnv_prime = 0x811C9DC5;
    unsigned int hash = 0;
    unsigned int i = 0;

    for (i = 0; i < length; str++, i++)
    {
        hash *= fnv_prime;
        hash ^= (*str);
    }

    return hash;
}

__global__
void unions_kernel(__device__ S* a, __device__ unsigned int* b) {
    //int i = gl_GlobalInvocationID.x;
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    S arr[16];
    for (int j = 0; j < 16; j++) {
        arr[j] = patch(a[j], i);
    }
    b[i] = hash_fnv((char*) &arr[0], sizeof(arr));
}

}
