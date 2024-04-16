#include <iostream>
#include <cassert>
#include <cmath>
#include <ctime>
#include <cstring>

extern "C" {
namespace shady {

#include "shady/runtime.h"
#include "shady/driver.h"

}

bool read_file(const char* filename, size_t* size, char** output);
}

typedef struct {
    size_t device;
} CommonAppArgs;

#define error_print printf
static void cli_parse_common_app_arguments(CommonAppArgs* args, int* pargc, char** argv) {
    int argc = *pargc;

    bool help = false;
    for (int i = 1; i < argc; i++) {
        if (argv[i] == NULL)
            continue;
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help = true;
            continue;
        } else if (strcmp(argv[i], "--device") == 0 || strcmp(argv[i], "-d") == 0) {
            argv[i] = NULL;
            i++;
            if (i >= argc) {
                error_print("Missing device number for --device\n");
                exit(1);
            }
            args->device = strtol(argv[i], NULL, 10);
        } else {
            continue;
        }
        argv[i] = NULL;
    }

    if (help) {
        error_print("Available arguments: \n");
        error_print("  --device n\n");
        exit(0);
    }

    shady::cli_pack_remaining_args(pargc, argv);
}

static uint64_t timespec_to_nano(struct timespec t) {
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

template<typename T>
static void fill_buffer(shady::Buffer* b, size_t size, bool zeroes = false) {
    T* tmp = (T*) calloc(sizeof(T), size);
    if (!zeroes) for (size_t i = 0; i < size; i++)
        tmp[i] = (T) i;
    shady::copy_to_buffer(b, 0, tmp, size * sizeof(T));
    free(tmp);
}

int main(int argc, char** argv) {
    CommonAppArgs common_args = {};
    shady::RuntimeConfig runtime_config = {};
    runtime_config.use_validation = true;
    runtime_config.dump_spv = true;
    shady::DriverConfig driver_config = shady::default_driver_config();
    shady::CompilerConfig& compiler_config = driver_config.config;
    compiler_config.hacks.restructure_everything = true;

    cli_parse_common_app_arguments(&common_args, &argc, argv);
    shady::cli_parse_driver_arguments(&driver_config, &argc, argv);
    shady::cli_parse_common_args(&argc, argv);
    shady::cli_parse_compiler_config_args(&compiler_config, &argc, argv);

    shady::Runtime* runtime = shady::initialize_runtime(runtime_config);
    shady::Device* device = shady::get_device(runtime, common_args.device);
    assert(device);

    size_t size;
    char* src;
    bool ok = read_file("main_gpu.ll", &size, &src);
    assert(ok);

    shady::Module* m;
    shady::driver_load_source_file(&compiler_config, shady::SrcLLVM, size, src, "m", &m);
    shady::Program* program = new_program_from_module(runtime, &compiler_config, m);

    size_t buffer_size = 0x1000000;

    shady::Buffer* buf_a = shady::allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_a, buffer_size);
    uint64_t buf_a_addr = shady::get_buffer_device_pointer(buf_a);

    shady::Buffer* buf_b = shady::allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_b, buffer_size);
    uint64_t buf_b_addr = shady::get_buffer_device_pointer(buf_a);

    shady::Buffer* buf_c = shady::allocate_buffer_device(device, buffer_size * sizeof(int32_t));
    fill_buffer<int32_t>(buf_c, buffer_size, true);
    uint64_t buf_c_addr = shady::get_buffer_device_pointer(buf_c);

    int nargs = 3;
    void* args[] = { &buf_a_addr, &buf_b_addr, &buf_c_addr };
    wait_completion(launch_kernel(program, device, "simple_iadd_global_buffers", buffer_size / 256, 1, 1, nargs, args));
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t tsn = timespec_to_nano(ts);
    wait_completion(launch_kernel(program, device, "simple_iadd_global_buffers", buffer_size / 256, 1, 1, nargs, args));
    struct timespec tp;
    timespec_get(&tp, TIME_UTC);
    uint64_t tpn = timespec_to_nano(tp);
    printf("kernel took %d us\n", (tpn - tsn) / 1000);

    int32_t* fin = (int32_t*) calloc(sizeof(int32_t), buffer_size);
    shady::copy_from_buffer(buf_c, 0, fin, buffer_size * sizeof(int32_t));
    printf("final values: \n");
    for (size_t i = 0; i < 32; i++)
        printf("%d ", fin[buffer_size - 32 + i]);
    printf("\n");
    return 0;
}
