#include <shady.h>
#include <stdint.h>

using namespace vcc;

extern "C" {

compute_shader local_size(256, 1, 1)
void simple_iadd_global_buffers(global int32_t* a, global int32_t* b, global int32_t* c) {
    int i = gl_GlobalInvocationID.x;
    c[i] = a[i] + b[i];
}

}