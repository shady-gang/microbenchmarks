#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

compute_shader local_size(256, 1, 1)
void add_chunked(int32_t* a, int32_t* b, int32_t* c) {
    int id = gl_GlobalInvocationID.x;
    for (int j = 0; j < 16; j++) {
        int i = id * 16 + j;
        c[i] = a[i] + b[i];
    }
}

}
