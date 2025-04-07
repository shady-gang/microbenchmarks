#include "bench.h"

void BENCH_NAME(shady::Runner* runtime, shady::Device* device, shady::CompilerConfig* compiler_config) {
    size_t size;
    char* src;
    bool ok = read_file(LL_FILE_NAME, &size, &src);
    assert(ok);

    shady::Module* m;
    shady::shd_driver_load_source_file(compiler_config, shady::SrcLLVM, size, src, "m", &m);
    shady::Program* program = shd_rn_new_program_from_module(runtime, compiler_config, m);

    size_t buffer_size = 0x1000000;

    shady::Buffer* buf_a = shady::shd_rn_allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_a, buffer_size);
    uint64_t buf_a_addr = shady::shd_rn_get_buffer_device_pointer(buf_a);

    shady::Buffer* buf_b = shady::shd_rn_allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_b, buffer_size);
    uint64_t buf_b_addr = shady::shd_rn_get_buffer_device_pointer(buf_b);

    shady::Buffer* buf_c = shady::shd_rn_allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_c, buffer_size, true);
    uint64_t buf_c_addr = shady::shd_rn_get_buffer_device_pointer(buf_c);

    int nargs = 3;
    void* args[] = { &buf_a_addr, &buf_b_addr, &buf_c_addr };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "add_chunked", (buffer_size / 256) / 16, 1, 1, nargs, args, NULL));
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t tsn = timespec_to_nano(ts);
    uint64_t profiled_gpu_time = 0;
    shady::ExtraKernelOptions extra_options = {
            .profiled_gpu_time = &profiled_gpu_time,
    };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "add_chunked",  (buffer_size / 256) / 16, 1, 1, nargs, args, &extra_options));
    struct timespec tp;
    timespec_get(&tp, TIME_UTC);
    uint64_t tpn = timespec_to_nano(tp);
    printf("kernel took %dus (gpu time: %dus)\n", (tpn - tsn) / 1000, profiled_gpu_time / 1000);

    int32_t* fin = (int32_t*) calloc(sizeof(int32_t), buffer_size);
    shady::shd_rn_copy_from_buffer(buf_c, 0, fin, buffer_size * sizeof(int32_t));
    printf("final values: \n");
    for (size_t i = 0; i < 32; i++)
        printf("%d ", fin[buffer_size - 32 + i]);
    printf("\n");
}
