@echo off
echo ====================================================================
echo   RUNNING PROBLEM SOLVING USING OOP (PSOOP) JAVA TESTS
echo ====================================================================
if not exist bin\psoop mkdir bin\psoop
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PSOOP Java sources.
    exit /b 1
)
java -cp bin\psoop test.PSOOPTestRunner
