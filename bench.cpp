#include "bench.h"

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

int main(int argc, char** argv) {
    CommonAppArgs common_args = {};
    shady::RuntimeConfig runtime_config = shady::default_runtime_config();
    shady::DriverConfig driver_config = shady::default_driver_config();
    shady::CompilerConfig& compiler_config = driver_config.config;
    compiler_config.hacks.restructure_everything = true;

    cli_parse_common_app_arguments(&common_args, &argc, argv);
    shady::cli_parse_driver_arguments(&driver_config, &argc, argv);
    shady::cli_parse_common_args(&argc, argv);
    shady::cli_parse_compiler_config_args(&compiler_config, &argc, argv);
    shady::cli_parse_runtime_config(&runtime_config, &argc, argv);

    shady::Runtime* runtime = shady::initialize_runtime(runtime_config);
    shady::Device* device = shady::get_device(runtime, common_args.device);
    assert(device);

    BENCH_NAME(runtime, device, &compiler_config);

    return 0;
}
