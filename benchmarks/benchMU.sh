#!/usr/bin/env bash

set -e

# -------------------------
# Arguments & defaults
# -------------------------
CODE_CPP="${1:-Dynamic_GomoryHu_Tree_MU.cpp}"
ITERATIONS="${2:-100}"
GEN_CPP="${3:-gen_MU.cpp}"

# -------------------------
# Files
# -------------------------
BASE_NAME=$(basename "$CODE_CPP" .cpp)
GEN_NAME=$(basename "$GEN_CPP" .cpp)

CSV_FILE="benchmark-$BASE_NAME-$GEN_NAME-MU.csv"

# -------------------------
# Compile
# -------------------------
CXXFLAGS="-std=c++20 -O3"

echo "Compiling..."
g++ $CXXFLAGS "../Implementation/Test/$CODE_CPP"  -o codeMU.out
g++ $CXXFLAGS "../Implementation/Test/$GEN_CPP"   -o genMU.out

# -------------------------
# CSV header
# -------------------------
if [ ! -f "$CSV_FILE" ]; then
    echo "Nodes,Create,Updates" > "$CSV_FILE"
fi

# -------------------------
# Benchmark loop
# -------------------------
printf "\n%s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s\n" "No." "Nodes" "Create(ms)" "Update1" "Update2" "Update3" "Update4" "Update5" "Update6" "Update7" "Update8" "Update9" "Update10"
  printf "%s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s\n" "---" "-----" "----------" "-------" "-------" "-------" "-------" "-------" "-------" "-------" "-------" "-------" "--------"

for ((i = 1; i <= ITERATIONS; ++i)); do
    # Generate input and output
    ./genMU.out "$i" > input_file

    OUTPUT=$(./codeMU.out < input_file)

    # Append CSV
    echo "$OUTPUT" >> "$CSV_FILE"

    # Print Output Partially (with alignment)
    IFS=',' read -r a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 _ <<< "$OUTPUT"

    printf " %-3d   %-5s   %-10s   %-7s   %-7s   %-7s   %-7s   %-7s   %-7s   %-7s   %-7s   %-7s   %-7s\n" \
           "$i"    "$a1"   "$a2"   "$a3"  "$a4"  "$a5"  "$a6"  "$a7"  "$a8"  "$a9"  "$a10" "$a11"  "$a12"
done

echo "🎉 Benchmarking complete. Results in $CSV_FILE"

echo "📊 Plotting results..."
python3 PlotterMU.py "$CSV_FILE"
