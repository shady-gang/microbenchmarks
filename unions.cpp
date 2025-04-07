#include "bench.h"
#include "unions.h"

void BENCH_NAME(shady::Runner* runtime, shady::Device* device, shady::CompilerConfig* compiler_config) {
    size_t size;
    char* src;
    bool ok = read_file(LL_FILE_NAME, &size, &src);
    assert(ok);

    shady::Module* m;
    shady::shd_driver_load_source_file(compiler_config, shady::SrcLLVM, size, src, "m", &m);
    shady::Program* program = shd_rn_new_program_from_module(runtime, compiler_config, m);

    //size_t buffer_size = 0x1000000;
    size_t a_size = 16;
    size_t b_size = 1024 * 1024;
    //b_size = 256;

    shady::Buffer* buf_a = shady::shd_rn_allocate_buffer_device(device, a_size * sizeof(S));
    S* s = (S*) calloc(sizeof(S), 16);
    s[0] = { .tag = 0, .i = 256 };
    s[1] = { .tag = 0, .i = 7 };
    s[2] = { .tag = 1, .f = 0.5f };
    s[3] = { .tag = 1, .f = 1.66f };
    s[4] = { .tag = 1, .f = 0.0001f };
    s[5] = { .tag = 1, .f = 7.7777f };
    s[6] = { .tag = 1, .f = 0.0f };
    s[7] = { .tag = 0, .i = 42 };
    s[8] = { .tag = 0, .i = 0 };
    s[9] = { .tag = 1, .f = 0.25f };
    s[10] = { .tag = 0, .i = 0x1000 };
    s[11] = { .tag = 1, .f = 10000000.f };
    s[12] = { .tag = 0, .i = -1 };
    s[13] = { .tag = 1, .f = -0.5f };
    s[14] = { .tag = 1, .f = -0.0f };
    s[15] = { .tag = 0, .i = -99999 };
    //fill_buffer<int32_t>(buf_a, buffer_size);
    shady::shd_rn_copy_to_buffer(buf_a, 0, s, 16 * sizeof(S));
    uint64_t buf_a_addr = shady::shd_rn_get_buffer_device_pointer(buf_a);

    shady::Buffer* buf_b = shady::shd_rn_allocate_buffer_device(device, b_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_b, b_size, true);
    uint64_t buf_b_addr = shady::shd_rn_get_buffer_device_pointer(buf_b);

    int nargs = 2;
    void* args[] = { &buf_a_addr, &buf_b_addr };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "unions_kernel", b_size / 256, 1, 1, nargs, args, NULL));
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t tsn = timespec_to_nano(ts);
    uint64_t profiled_gpu_time = 0;
    shady::ExtraKernelOptions extra_options = {
            .profiled_gpu_time = &profiled_gpu_time,
    };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "unions_kernel", b_size / 256, 1, 1, nargs, args, &extra_options));
    struct timespec tp;
    timespec_get(&tp, TIME_UTC);
    uint64_t tpn = timespec_to_nano(tp);
    printf("kernel took %dus (gpu time: %dus)\n", (tpn - tsn) / 1000, profiled_gpu_time / 1000);

    uint32_t* fin = (uint32_t*) calloc(sizeof(float), b_size);
    shady::shd_rn_copy_from_buffer(buf_b, 0, fin, b_size * sizeof(int32_t));
    printf("final valuesz: \n");
    for (size_t i = 0; i < 32; i++)
        printf("%x\n", fin[b_size - 32 + i]);
    printf("\n");
}
