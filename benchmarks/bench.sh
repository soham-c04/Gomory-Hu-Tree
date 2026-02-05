#!/usr/bin/env bash

set -e

# -------------------------
# Arguments & defaults
# -------------------------
CODE_CPP="$1"
ITERATIONS="${2:-100}"
BRUTE_CPP="${3:-brute.cpp}"
GEN_CPP="${4:-gen.cpp}"

if [ -z "$CODE_CPP" ]; then
    echo "Usage: ./bench.sh <code.cpp> [iterations] [brute.cpp] [gen.cpp]"
    exit 1
fi

# -------------------------
# Files
# -------------------------
BASE_NAME=$(basename "$CODE_CPP" .cpp)
BRUTE_NAME=$(basename "$BRUTE_CPP" .cpp)
GEN_NAME=$(basename "$GEN_CPP" .cpp)

CSV_FILE="benchmark-$BASE_NAME-$BRUTE_NAME-$GEN_NAME.csv"

# -------------------------
# Compile
# -------------------------
CXXFLAGS="-std=c++20 -O3"

echo "Compiling..."
g++ $CXXFLAGS "../Implementation/$CODE_CPP"  -o code.out
g++ $CXXFLAGS "../Implementation/Test/$BRUTE_CPP" -o brute.out
g++ $CXXFLAGS "../Implementation/Test/$GEN_CPP"   -o gen.out

# -------------------------
# CSV header
# -------------------------
if [ ! -f "$CSV_FILE" ]; then
    echo "nodes,code,brute" > "$CSV_FILE"
fi

# -------------------------
# Benchmark loop
# -------------------------
printf "\n%-10s\t%-10s\t%-10s\t%-10s\n" "Iteration" "Nodes" "Code(ms)" "Brute(ms)"
printf "%-10s\t%-10s\t%-10s\t%-10s\n" "---------" "-----" "--------" "---------"

for ((i = 1; i <= ITERATIONS; ++i)); do
    # Generate input
    ./gen.out "$i" > input_file

    # Extract first integer (number of nodes)
    NODES=$(head -n 1 input_file | awk '{print $1}')

    CODE_MS=$(./code.out 0 < input_file | awk '{print $1}')
    BRUTE_MS=$(./brute.out < input_file | awk '{print $1}')

    # Append CSV
    echo "$NODES,$CODE_MS,$BRUTE_MS" >> "$CSV_FILE"
    printf "%-10d\t%-10d\t%-10d\t%-10d\n" "$i" "$NODES" "$CODE_MS" "$BRUTE_MS"
done

echo "🎉 Benchmarking complete. Results in $CSV_FILE"

echo "📊 Plotting results..."
python3 Plotter.py "$CSV_FILE"
