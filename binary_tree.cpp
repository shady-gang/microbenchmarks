#include "bench.h"

#define FN_PREFIX
#include "binary_tree.h"

unsigned int FNVHash(char* str, unsigned int length) {
    const unsigned int fnv_prime = 0x811C9DC5;
    unsigned int hash = 0;
    unsigned int i = 0;

    for (i = 0; i < length; str++, i++)
    {
        hash *= fnv_prime;
        hash ^= (*str);
    }

    return hash;
}

using Node = TreeNode<int>;

void BENCH_NAME(shady::Runtime* runtime, shady::Device* device, shady::CompilerConfig* compiler_config) {
    size_t size;
    char* src;
    bool ok = read_file(LL_FILE_NAME, &size, &src);
    assert(ok);

    shady::Module* m;
    shady::driver_load_source_file(compiler_config, shady::SrcLLVM, size, src, "m", &m);
    shady::Program* program = new_program_from_module(runtime, compiler_config, m);

    size_t buffer_size = 0x1000000;
    int max = 50000;
    int* numbers = (int*) calloc(buffer_size, sizeof(uint32_t));
    Node* nodes = (Node*) calloc(buffer_size, sizeof(Node));
    Node* root = nullptr;
    for (size_t i = 0; i < buffer_size; i++) {
        size_t r = i + 48;
        numbers[i] = FNVHash((char*) &r, sizeof(size_t)) % max;
        nodes[i].key = numbers[i];
        root = insert<int>(root, &nodes[i]);
    }

    int max_depth = 0;
    for (int n = 0; n < max; n++) {
        Node* found = find(root, n);
        if (found) {
            //printf("found %d at depth %d\n", n, found->depth);
            if (found->depth > max_depth)
                max_depth = found->depth;
        }
    }
    printf("max depth %d\n", max_depth);

    shady::Buffer* buf_a = shady::allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    shady::copy_to_buffer(buf_a, 0, numbers, buffer_size * sizeof(uint32_t));
    uint64_t buf_a_addr = shady::get_buffer_device_pointer(buf_a);

    shady::Buffer* buf_b = shady::allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    uint64_t buf_b_addr = shady::get_buffer_device_pointer(buf_b);

    shady::Buffer* buf_nodes = shady::allocate_buffer_device(device, buffer_size * sizeof(Node));
    uint64_t buf_nodes_addr = shady::get_buffer_device_pointer(buf_nodes);
    // patch the addresses of the tree nodes
    uint64_t patch = buf_nodes_addr - (uint64_t) nodes;
    for (size_t i = 0; i < buffer_size; i++) {
        if (nodes[i].children[0])
            nodes[i].children[0] = (Node*) (((uint64_t) nodes[i].children[0]) + patch);
        if (nodes[i].children[1])
            nodes[i].children[1] = (Node*) (((uint64_t) nodes[i].children[1]) + patch);
    }
    shady::copy_to_buffer(buf_nodes, 0, nodes, buffer_size * sizeof(Node));

    int nargs = 3;
    uint64_t root_addr = buf_nodes_addr + ((uint64_t)root - (uint64_t)nodes);
    void* args[] = { &buf_a_addr, &buf_b_addr, &root_addr };
    wait_completion(launch_kernel(program, device, "treesearch", buffer_size / 256, 1, 1, nargs, args, NULL));
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t tsn = timespec_to_nano(ts);
    uint64_t profiled_gpu_time = 0;
    shady::ExtraKernelOptions extra_options = {
            .profiled_gpu_time = &profiled_gpu_time,
    };
    wait_completion(launch_kernel(program, device, "treesearch", buffer_size / 256, 1, 1, nargs, args, &extra_options));
    struct timespec tp;
    timespec_get(&tp, TIME_UTC);
    uint64_t tpn = timespec_to_nano(tp);
    printf("kernel took %dus (gpu time: %dus)\n", (tpn - tsn) / 1000, profiled_gpu_time / 1000);

    int32_t* fin = (int32_t*) calloc(sizeof(int32_t), buffer_size);
    shady::copy_from_buffer(buf_b, 0, fin, buffer_size * sizeof(int32_t));
    printf("final values: \n");
    for (size_t i = 0; i < 32; i++)
        printf("%d ", fin[buffer_size - 32 + i]);
    printf("\n");
}
