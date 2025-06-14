#ifndef BENCH_H
#define BENCH_H

#include <cassert>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

extern "C" {
namespace shady {

#include "shady/runner/runner.h"
#include "shady/driver.h"

}

bool read_file(const char* filename, size_t* size, char** output);
}

static uint64_t timespec_to_nano(struct timespec t) {
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

typedef struct {
    size_t device;
} CommonAppArgs;

template<typename T>
inline static void fill_buffer(shady::Buffer* b, size_t size, bool zeroes = false) {
    T* tmp = (T*) calloc(sizeof(T), size);
    if (!zeroes) for (size_t i = 0; i < size; i++)
            tmp[i] = (T) i;
    shady::shd_rn_copy_to_buffer(b, 0, tmp, size * sizeof(T));
    free(tmp);
}

void BENCH_NAME(shady::Runner* runtime, shady::Device* device, shady::CompilerConfig* compiler_config);

#endif BENCH_H