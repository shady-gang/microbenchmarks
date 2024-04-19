#include <shady.h>
#include <stdint.h>

using namespace vcc;

#include "binary_tree.h"

extern "C" {

using Node = TreeNode<int>;

compute_shader local_size(256, 1, 1)
void treesearch(global int32_t* a, global int32_t* b, global Node* root) {
    int i = gl_GlobalInvocationID.x;
    b[i] = !!find((Node*) root, a[i]);
}

}
