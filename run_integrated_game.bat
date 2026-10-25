@echo off
echo ====================================================================
echo    AIRSTRIKER: 4-SUBJECT REAL-TIME INTEGRATED LAUNCHER              
echo    PL (C++) + COA (80386 ALU) + CGL (OpenGL) + PSOOP (Java Engine)  
echo ====================================================================

REM Ensure bin and bin\psoop exist
if not exist bin mkdir bin
if not exist bin\psoop mkdir bin\psoop

REM Compile Java
echo [1/3] Compiling PSOOP Java Subsystem...
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PSOOP Java sources.
    pause
    exit /b 1
)

REM Compile Native Game
echo [2/3] Compiling Integrated Game (CGL + PL + COA + NetworkBridge)...
g++ -std=c++11 -I./PL/src -I./COA/ALU -I./CGL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/src/NetworkBridge.cpp PL/src/BulletPoolArray.cpp PL/src/EnemyLinkedList.cpp PL/src/AsteroidGridMatrix.cpp COA/ALU/ALU80386.cpp CGL/src/MainGame.cpp -o bin/AirStriker_CO1.exe -lopengl32 -lglu32 -lglut32 -lws2_32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL game executable.
    pause
    exit /b 1
)

REM Start Java Server in dedicated visible console window
echo [3/3] Starting PSOOP Java Domain Server in separate console...
start "AirStriker - PSOOP Domain Server" java -cp bin/psoop service.PSOOPBridgeServer

REM Wait 1 second for port binding
timeout /t 1 /nobreak >nul

REM Launch Game
echo >>> Launching 4-Subject Integrated Desktop Game Window <<<
bin\AirStriker_CO1.exe
