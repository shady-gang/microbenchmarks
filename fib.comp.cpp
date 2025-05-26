#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

void debug_printflli(const char*, long long, int) __asm__("shady::instruction::DebugPrintf");

int get_stack_size(void) __asm__("shady::prim_op::get_stack_size");

int fib(int n) {
    //debug_printfi("n=%d\n", n);
    //debug_printfi("ss=%d\n", get_stack_size());
    if (n <= 1)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

compute_shader local_size(256, 1, 1)
void fib_main(int32_t* a) {
    int i = gl_GlobalInvocationID.x;
    a[i] = fib(i % 16);
}

}
