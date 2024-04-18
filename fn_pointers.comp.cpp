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
    int (*fns[4])(int, int) = { f, g, h, k };
    int (*fn)(int, int) = fns[i % 4];
    //debug_printfll("fn: %lu", (long long) fn);

    c[i] = fn(a[i], b[i]) + 41;
}

}
