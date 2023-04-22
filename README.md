# Project assignment 1 - CFG Printer

## Students

Guilherme O. Silva - 2023671528

Henrique F. Scheid - 2017014898

## Environment

Requirements:

* LLVM 16.0.1
* CMake 3.26.3

Tested with

* macOS Ventura 13.2.1 (Apple M1)
* Ubuntu 18.04.6

## Setup

### Export the following environment variables

1. `LLVM_PATH`

```
export LLVM_PATH=/path/to/llvm
```

example:
```
export LLVM_PATH=/home/linuxbrew/.linuxbrew/bin
```

2. `CLANG_PATH`

```
export CLANG_PATH=/path/to/clang/callable
```

example:
```
export CLANG_PATH=/home/linuxbrew/.linuxbrew/bin/clang
```

3. `OPT_PATH`

```
export OPT_PATH=/path/to/opt/callable
```

example:
```
export OPT_PATH=/home/linuxbrew/.linuxbrew/bin/opt
```

4. `LIB_EXTENSION`

`clang` will compile the pass to a `.so` file if using Linux, or `.dylib` if using macOS. Thus, export the following variable according to your OS.

```
# if using macOS
export LIB_EXTENSION=dylib

# if using Linux
export LIB_EXTENSION=so
```

### Run `run.sh`

From a terminal window pointed to the root of this project, simply call

```
bash run.sh
```

This file will

1. Create the `build/` and `results/` directories, to contain the compiled pass and the results of the pass, respectivelly.
2. Build the LLVM pass and output the compiled library to `build/libCFGModulePass.{so|dylib}`.
3. Generate LLVM Intermediate Representations for each source in `benchmarks/Stanford`, in `.ll` format, with `clang`. These files will be output to `build/benchmarks/`.
4. Run the pass over the built benchmarks, and output the `.dot` files to `results/dots/`.

## Acknowledgements

We thank the [llvm-tutor](https://github.com/banach-space/llvm-tutor) project for providing a template for creating our pass, and many useful resources to develop it.
