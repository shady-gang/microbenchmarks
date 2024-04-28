#include <shady.h>
#include <stdint.h>

using namespace vcc;

#define FN_PREFIX
#include "unions.h"

#define w 256

extern "C" {

void debug_printff(const char*, float) __asm__("shady::prim_op::debug_printf::i");
void debug_printfi(const char*, int) __asm__("shady::prim_op::debug_printf::i");

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

compute_shader local_size(256, 1, 1)
void unions_kernel(S* a, global unsigned int* b) {
    int i = gl_GlobalInvocationID.x;
    S arr[16];
    for (int j = 0; j < 16; j++) {
        arr[j] = patch(a[j], i);
    }

    int rounds = 8;
    unsigned int acc = 0;
    for (int k = 0; k < rounds; k++) {
        for (int j = 0; j < 16; j++)
            arr[j] = patch(arr[j], i);
        acc ^= hash_fnv((char*) &arr[0], sizeof(arr));
    }
    b[i] = acc;
}

}
