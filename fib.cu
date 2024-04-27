#include <cstdint>

extern "C" {

__device__ int fib(int n) {
    if (n <= 1)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

struct Fib {
    int fib_stack_size = 0;
    int fib_stack[32];

    __device__ int fib_stack_unwind(int r) {
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

    __device__ int fib_(int r, int n) {
        fib_stack[fib_stack_size++] = r;
        fib_stack[fib_stack_size++] = 1;
        return fib(n);
    }

    __device__ int fib(int n) {
        if (n <= 1)
            return fib_stack_unwind(1);
        fib_stack[fib_stack_size++] = n - 2;
        fib_stack[fib_stack_size++] = 0;
        return fib(n - 1);
    }

    /*__device__ int fib(int n) {
        int r;

        while (1) {
            //debug_printfi("n=%d\n", n);
            if (n <= 1) {
                //debug_printfi("unwind, old_r=%d\n", r);
                r = 1;
                bool unwind = true;
                while (unwind) {
                    //debug_printfi("unwind iter, fib_stack_size=%d\n", fib_stack_size);
                    if (fib_stack_size > 0) {
                        int t = fib_stack[--fib_stack_size];
                        switch (t) {
                            case 0: {
                                n = fib_stack[--fib_stack_size];
                                //debug_printfi("popped n=%d\n", n);
                                fib_stack[fib_stack_size++] = r;
                                fib_stack[fib_stack_size++] = 1;
                                unwind = false;
                                break;
                            }
                            case 1: {
                                int r2 = fib_stack[--fib_stack_size];
                                r = r + r2;
                                //debug_printfi("r=%d\n", r);
                                break;
                            }
                        }
                        continue;
                    }
                    //debug_printfi("return=%d\n", r);
                    return r;
                }
                continue;
            }
            fib_stack[fib_stack_size++] = n - 2;
            fib_stack[fib_stack_size++] = 0;
            n = n - 1;
        }
    }*/
};

__global__ void fib_main(__device__ int32_t* a) {
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    Fib f {};
    a[i] = f.fib(i % 16);
}

}