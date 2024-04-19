# vcc/shady microbenchmarks

## How to use

```
mkdir build
cd build
cmake .. -Dshady_DIR=<SHADY_INSTALL_DIR>/share/cmake/shady
make

./<bench> --device N
```

## How to try the CUDA variants:

```
nvcc <bench>.cu -c -ptx
SHADY_OVERRIDE_PTX=<bench>.cu ./<bench> --device <CUDA_DEVICE>
```

doesn't do anything if the device isn't cuda-based!
