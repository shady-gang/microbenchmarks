#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

private int fib_stack_size = 0;
private int fib_stack[32];

static int fib(int n) {
    int r;

    while (1) {
        if (n <= 1) {
            r = 1;
            bool unwind = true;
            while (unwind) {
                if (fib_stack_size > 0) {
                    int t = fib_stack[--fib_stack_size];
                    switch (t) {
                        case 0: {
                            n = fib_stack[--fib_stack_size];
                            fib_stack[fib_stack_size++] = r;
                            fib_stack[fib_stack_size++] = 1;
                            unwind = false;
                            break;
                        }
                        case 1: {
                            int r2 = fib_stack[--fib_stack_size];
                            r = r + r2;
                            break;
                        }
                    }
                    continue;
                }
                return r;
            }
            continue;
        }
        fib_stack[fib_stack_size++] = n - 2;
        fib_stack[fib_stack_size++] = 0;
        n = n - 1;
    }
}

compute_shader local_size(256, 1, 1)
void fib_main(global int32_t* a) {
    int i = gl_GlobalInvocationID.x;
    a[i] = fib(i % 16);
}

}
