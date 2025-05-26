#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

int f(int x, int y) {
    return x + y;
}

int g(int x, int y) {
    return x * y;
}

int h(int x, int y) {
    return x - y;
}

int k(int x, int y) {
    if (y == 0)
        return x;
    return x / y;
}

void debug_printflli(const char*, long long, int) __asm__("shady::instruction::DebugPrintf");

int (*fns[4])(int, int) = { f, g, h, k };

compute_shader local_size(32, 1, 1)
void fn_ptrs(global int32_t* a, global int32_t* b, global int32_t* c) {
    int i = gl_GlobalInvocationID.x;
    int (*fn)(int, int) = fns[i % 4];
    c[i] = fn(a[i], b[i]) + 41;
}

}
