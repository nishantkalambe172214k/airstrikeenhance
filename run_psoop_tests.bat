@echo off
echo ====================================================================
echo   RUNNING PROBLEM SOLVING USING OOP (PSOOP) JAVA TESTS
echo ====================================================================
if not exist bin\psoop\test\PSOOPTestRunner.class (
    call build_all.bat
)
java -cp bin\psoop test.PSOOPTestRunner
