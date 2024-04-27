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

void debug_printfll(const char*, long long) __asm__("shady::prim_op::debug_printf::ll");

compute_shader local_size(256, 1, 1)
void fn_ptrs(global int32_t* a, global int32_t* b, global int32_t* c) {
    int i = gl_GlobalInvocationID.x;
    switch (i % 4) {
        case 0: c[i] = f(a[i], b[i]) + 41; return;
        case 1: c[i] = g(a[i], b[i]) + 41; return;
        case 2: c[i] = h(a[i], b[i]) + 41; return;
        case 3: c[i] = k(a[i], b[i]) + 41; return;
    }
}

}
