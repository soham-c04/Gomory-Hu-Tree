#!/usr/bin/env bash

set -e

# -------------------------
# Arguments & defaults
# -------------------------
CODE_CPP="$1"
BRUTE_CPP="${2:-brute.cpp}"
GEN_CPP="${3:-gen.cpp}"

if [ -z "$CODE_CPP" ]; then
    echo "Usage: ./bash.sh <code.cpp> [brute.cpp] [gen.cpp]"
    exit 1
fi

# -------------------------
# Compile
# -------------------------
CXXFLAGS="-std=c++20 -O3"

echo "Compiling..."
g++ $CXXFLAGS "../Implementation/$CODE_CPP"  -o code.out
g++ $CXXFLAGS "../Implementation/Test/$BRUTE_CPP" -o brute.out
g++ $CXXFLAGS "../Implementation/Test/$GEN_CPP"   -o gen.out

# -------------------------
# Stress Test loop
# -------------------------

for((i = 1; ; ++i)); do
    ./gen.out $i > input_file
    ./code.out 1 < input_file > myAnswer
    ./brute.out 1 < input_file > correctAnswer
#    cat input_file
#echo "St: "
#    cat correctAnswer
#echo "End"
    diff -Z myAnswer correctAnswer > /dev/null || break
    echo "Passed test: "  $i
done
echo "	|"
echo "	|"
echo "	|"
echo "	|"
echo "	|"
echo "	|"
echo "	|"
echo "WA on the following test:"
cat input_file
echo "Your answer is:"
cat myAnswer
echo "Correct answer is:"
cat correctAnswer
export PATH="/mnt/c/mingw/bin:$PATH"
