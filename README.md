These programs are demonstrations of how LLVM can be used for (very simple)
static and dynamic analyses. However, they use many of the initial building
blocks necessary for solving more complex problems.

The provided analyses count the number of direct invocations of a particular
function are present within a program or within and execution depending on
whether static or dynamic analysis is used.

Building with CMake
==============================================
## To run

- Add conda to your bashrc
```bash
. "/data/miniconda3/etc/profile.d/conda.sh"
```

```bash
conda activate
```
```bash
export PATH=/data/rvv/bin/:/data/rvv-llvm/bin:$PATH 
# This step can fail if you don't have wllvm in the path or you have not included gnu and clang in your path
# LLVM_COMPILER is NOT A PATH to clang IT IS THE NAME OF THE CLANG EXECUTABLE
LLVM_COMPILER=clang BINUTILS_TARGET_PREFIX=riscv64-unknown-elf make 
```

1. Clone the demo repository.

        git clone git@github.com:sfu-arch/llvm-demo.git


2. Create a new directory for building.

        mkdir demobuild

3. Change into the new directory.

        cd demobuild

4. Run CMake with the path to the LLVM source.

        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=True \
            -DLLVM_DIR=</path/to/LLVM/build>/lib/cmake/llvm/ ../llvm-demo
To use `LLVM-11` on server-01:
```
-DLLVM_DIR=/data/llvm-11/lib/cmake/llvm
```
5. Run make inside the build directory:

        make

This produces a callcounter tool called bin/callcounter and supporting
libraries in lib/.

Note, building with a tool like ninja can be done by adding `-G Ninja` to
the cmake invocation and running ninja instead of make.

Running
==============================================

First suppose that you have a program compiled to bitcode:

    clang -g -c -emit-llvm ../llvm-demo/test/simpletest.c -o calls.bc

Running the dynamic call printer:

    bin/callcounter -dynamic calls.bc -o calls
    ./calls

Running the static call printer:

    bin/callcounter -static calls.bc

or by loading the pass as a plugin for `opt`:

    opt -analyze -load lib/libcallcounter-lib.so -callcounter calls.bc
