#!/bin/bash
# ==============================================================================
# [COA CO3] AIR STRIKER: COA 64-BIT ALU & ASSEMBLY TEST RUNNER
# Course Outcome 3: Algorithmic and String Processing
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "===================================================================="
echo "  RUNNING COA 64-BIT ALU & ASSEMBLY TEST SUITE (macOS / Linux)     "
echo "  Course Outcome 3: Algorithmic and String Processing               "
echo "===================================================================="

mkdir -p bin

# Determine NASM path
NASM_BIN="nasm"
if command -v /opt/homebrew/bin/nasm >/dev/null 2>&1; then
    NASM_BIN="/opt/homebrew/bin/nasm"
elif ! command -v nasm >/dev/null 2>&1; then
    echo "ERROR: nasm is not installed or not in PATH."
    exit 1
fi

echo "[1/4] Assembling COA CO3 64-bit Arithmetic Assembly (Shift-Add & Successive)..."
"$NASM_BIN" -f macho64 COA/ALP/coa_co3_arithmetic.asm -o COA/ALP/coa_co3_arithmetic.o

echo "[2/4] Assembling COA CO3 64-bit String Processing Assembly (Length, Upper, Cmp)..."
"$NASM_BIN" -f macho64 COA/ALP/coa_co3_string.asm -o COA/ALP/coa_co3_string.o

echo "[3/4] Compiling and linking COA CO3 unit tests..."
clang++ -arch x86_64 -std=c++11 -I./COA/ALU -I./COA/ALP \
    COA/ALU/ALU80386.cpp \
    COA/ALU/test_coa_co3.cpp \
    COA/ALP/coa_co3_arithmetic.o \
    COA/ALP/coa_co3_string.o \
    -o bin/test_coa_co3

echo "[4/4] Executing COA CO3 Test Suite via Rosetta 2 (arch -x86_64)..."
arch -x86_64 ./bin/test_coa_co3

echo ""
echo ">>> All COA CO3 64-bit ALU and Assembly tests executed successfully! <<<"
