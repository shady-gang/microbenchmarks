#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

compute_shader local_size(256, 1, 1)
void add_buffers(int32_t* a, int32_t* b, int32_t* c) {
    int i = gl_GlobalInvocationID.x;
    c[i] = a[i] + b[i];
}

}
