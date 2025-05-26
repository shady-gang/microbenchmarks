#include "bench.h"

void BENCH_NAME(shady::Runner* runtime, shady::Device* device, shady::CompilerConfig* compiler_config) {
    //compiler_config->hacks.restructure_everything = false;

    size_t size;
    char* src;
    bool ok = read_file(LL_FILE_NAME, &size, &src);
    assert(ok);

    shady::Module* m;
    shady::TargetConfig target_config = shady::shd_default_target_config();
    shady::shd_driver_load_source_file(compiler_config, &target_config, shady::SrcLLVM, size, src, "m", &m);
    shady::Program* program = shd_rn_new_program_from_module(runtime, compiler_config, m);

    size_t wg_size = 256;
    size_t buffer_size = wg_size * 1024 * 4;

    shady::Buffer* buf_a = shady::shd_rn_allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_a, buffer_size, true);
    uint64_t buf_a_addr = shady::shd_rn_get_buffer_device_pointer(buf_a);

    int nargs = 1;
    void* args[] = { &buf_a_addr };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "fib_main", buffer_size / wg_size, 1, 1, nargs, args, NULL));
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t tsn = timespec_to_nano(ts);
    uint64_t profiled_gpu_time = 0;
    shady::ExtraKernelOptions extra_options = {
            .profiled_gpu_time = &profiled_gpu_time,
    };
    shd_rn_wait_completion(shd_rn_launch_kernel(program, device, "fib_main", buffer_size / wg_size, 1, 1, nargs, args, &extra_options));
    struct timespec tp;
    timespec_get(&tp, TIME_UTC);
    uint64_t tpn = timespec_to_nano(tp);
    printf("kernel took %dus (gpu time: %dus)\n", (tpn - tsn) / 1000, profiled_gpu_time / 1000);

    int32_t* fin = (int32_t*) calloc(sizeof(int32_t), buffer_size);
    shady::shd_rn_copy_from_buffer(buf_a, 0, fin, buffer_size * sizeof(int32_t));
    printf("final values: \n");
    for (size_t i = 0; i < 32; i++)
        printf("%d ", fin[buffer_size - 32 + i]);
    printf("\n");
}
