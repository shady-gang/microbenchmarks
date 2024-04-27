#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

private int fib_stack_size = 0;
private int fib_stack[32];

int fib_(int r, int n);
int fib(int n);

int fib_stack_unwind(int r) {
    if (fib_stack_size > 0) {
        int t = fib_stack[--fib_stack_size];
        switch (t) {
            case 0: {
                int n = fib_stack[--fib_stack_size];
                return fib_(r, n);
            }
            case 1: {
                int r2 = fib_stack[--fib_stack_size];
                return fib_stack_unwind(r + r2);
            }
        }
    }
    return r;
}

int fib_(int r, int n) {
    fib_stack[fib_stack_size++] = r;
    fib_stack[fib_stack_size++] = 1;
    return fib(n);
}

int fib(int n) {
    if (n <= 1)
        return fib_stack_unwind(1);
    fib_stack[fib_stack_size++] = n - 2;
    fib_stack[fib_stack_size++] = 0;
    return fib(n - 1);
}

compute_shader local_size(256, 1, 1)
void fib_main(global int32_t* a) {
int i = gl_GlobalInvocationID.x;
a[i] = fib(i % 16);
}

}
