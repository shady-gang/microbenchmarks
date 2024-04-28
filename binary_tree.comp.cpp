#include <shady.h>
#include <stdint.h>

using namespace vcc;

#define FN_PREFIX
#include "binary_tree.h"

extern "C" {

using Node = TreeNode<int>;

compute_shader local_size(256, 1, 1)
void treesearch(global int32_t* a, global int32_t* b, global Node* root) {
    int i = gl_GlobalInvocationID.x;
    int rounds = 1;
    for (int k = 0; k < rounds; k++) {
        b[i] = !!find((Node*) root, a[i]);
        i = (i + 1) % 0x10000;
    }
}

}
