set -e

echo "ENVIRONMENT"
echo "Path to LLVM: $LLVM_PATH"
echo "Path to clang: $CLANG_PATH"
echo "Path to opt: $OPT_PATH"
echo "Library extension format: $LIB_EXTENSION"

echo ""
echo "Creating dependencies folders..."

mkdir -p results
mkdir -p results/dots
mkdir -p build
mkdir -p build/benchmarks

echo ""
echo "Building the LLVM pass..."

cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_PATH ../lib/
make
cd ..

echo ""
echo "Compiling benchmarks and running the pass..."

BENCHMARKS=(
    "Bubblesort"
    "IntMM"
    "Perm"
    "Queens"
    "RealMM"
    "Treesort"
    "FloatMM"
    "Oscar"
    "Puzzle"
    "Quicksort"
    "Towers"
)

for bench in "${BENCHMARKS[@]}"
do
    echo "Running the pass for bench: $bench"

    $CLANG_PATH -Wno-everything -S -emit-llvm benchmarks/Stanford/"$bench.c" -o build/benchmarks/"$bench.ll"
    $OPT_PATH -load-pass-plugin "./build/libCFGModulePass.$LIB_EXTENSION" -passes="cfg-module-pass" build/benchmarks/"$bench.ll" -disable-output

    mv "$bench".dot results/dots/
done
