#include <shady.h>
#include <stdint.h>

using namespace vcc;

#include "binary_tree.h"

extern "C" {
void debug_printfi(const char*, int) __asm__("shady::prim_op::debug_printf::i");
void debug_printfill(const char*, int, long long, long long) __asm__("shady::prim_op::debug_printf::ill");
void debug_printfii(const char*, int, int) __asm__("shady::prim_op::debug_printf::ii");
void debug_printfiil(const char*, int, int, long long) __asm__("shady::prim_op::debug_printf::iil");
void debug_printfiiil(const char*, int, int, int, long long) __asm__("shady::prim_op::debug_printf::iiil");
int get_stack_size(void) __asm__("shady::prim_op::get_stack_size");
}

template<typename K>
TreeNode<K>* find2(TreeNode<K>* tree, K value) {
    debug_printfiiil("n=%d pivot=%d ss=%d tree=%lu\n", value, tree->key, get_stack_size(), (long long) tree);
    if (tree->key == value)
        return tree;
    if (value < tree->key && tree->children[0])
        return find2(tree->children[0], value);
    if (value > tree->key && tree->children[1])
        return find2(tree->children[1], value);
    debug_printfill("giving up on %d %lu %lu\n", value, (long long) tree->children[0], (long long) tree->children[1]);
    return nullptr;
}

extern "C" {

using Node = TreeNode<int>;

compute_shader local_size(32, 1, 1)
void treesearch(global int32_t* a, global int32_t* b, global Node* root) {
    int i = gl_GlobalInvocationID.x;
    debug_printfiil("%d find %d %lu\n", i, a[i], (long long) root);
    b[i] = !!find2((Node*) root, a[i]);
    debug_printfii("found %d %d\n", a[i], b[i]);
}

}
