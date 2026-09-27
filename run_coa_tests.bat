@echo off
echo ====================================================================
echo   RUNNING COMPUTER ORGANIZATION AND ARCHITECTURE (COA) ALU TESTS
echo ====================================================================
if not exist bin\test_alu.exe (
    call build_all.bat
)
"bin\test_alu.exe"
