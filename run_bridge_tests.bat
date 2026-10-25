@echo off
echo ====================================================================
echo    AIR STRIKER: 4-SUBJECT REAL-TIME BRIDGE VERIFICATION            
echo ====================================================================

if not exist bin mkdir bin
if not exist bin\psoop mkdir bin\psoop

echo [1/3] Compiling Java...
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Java compilation failed.
    exit /b 1
)

echo [2/3] Compiling C++ Bridge Test...
g++ -std=c++11 -I./CGL/src CGL/src/NetworkBridge.cpp CGL/tests/test_bridge_integration.cpp -o bin/test_bridge_integration.exe -lws2_32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] C++ test compilation failed.
    exit /b 1
)

echo [3/3] Starting PSOOP Bridge Server and running automated test...
start /b java -cp bin/psoop service.PSOOPBridgeServer >nul 2>&1
timeout /t 1 /nobreak >nul

bin\test_bridge_integration.exe
