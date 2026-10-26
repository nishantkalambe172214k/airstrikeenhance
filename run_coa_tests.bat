@echo off
rem ==============================================================================
rem [COA CO3] AIR STRIKER: COA 64-BIT ALU & ASSEMBLY TEST RUNNER (WINDOWS)
rem Course Outcome 3: Algorithmic and String Processing
rem ==============================================================================
echo ====================================================================
echo   RUNNING COA 64-BIT ALU & ASSEMBLY TEST SUITE (Windows x64)
echo   Course Outcome 3: Algorithmic and String Processing
echo ====================================================================

if not exist bin mkdir bin

echo [1/4] Assembling COA CO3 64-bit Arithmetic Assembly...
nasm -f win64 COA\ALP\coa_co3_arithmetic.asm -o COA\ALP\coa_co3_arithmetic.obj

echo [2/4] Assembling COA CO3 64-bit String Processing Assembly...
nasm -f win64 COA\ALP\coa_co3_string.asm -o COA\ALP\coa_co3_string.obj

echo [3/4] Compiling COA CO3 Unit Tests with g++...
g++ -std=c++11 -I./COA/ALU -I./COA/ALP COA/ALU/ALU80386.cpp COA/ALU/test_coa_co3.cpp COA/ALP/coa_co3_arithmetic.obj COA/ALP/coa_co3_string.obj -o bin\test_coa_co3.exe

echo [4/4] Executing test_coa_co3.exe...
"bin\test_coa_co3.exe"
