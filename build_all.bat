@echo off
echo ====================================================================
echo   BUILDING AIRSTRIKER — 2D SPACE SHOOTER (CO-1 ^& CO-2)
echo ====================================================================

if not exist bin mkdir bin
if not exist bin\psoop mkdir bin\psoop

echo [1/8] Compiling Programming Lab (PL) Bullet Pool Tests (CO-1)...
g++ -std=c++11 -I./PL/src PL/src/BulletPoolArray.cpp PL/tests/test_bullet_pool.cpp -o bin/test_bullet_pool.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL bullet pool tests.
    exit /b 1
)

echo [2/8] Compiling Programming Lab (PL) Enemy Linked List Tests (CO-2)...
g++ -std=c++11 -I./PL/src PL/src/EnemyLinkedList.cpp PL/tests/test_enemy_list.cpp -o bin/test_enemy_list.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL enemy linked list tests.
    exit /b 1
)

echo [3/12] Compiling Programming Lab (PL) 2D Array Asteroid Grid Matrix Tests (CO-2)...
g++ -std=c++11 -I./PL/src PL/src/AsteroidGridMatrix.cpp PL/tests/test_asteroid_grid.cpp -o bin/test_asteroid_grid.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL asteroid grid matrix tests.
    exit /b 1
)

echo [4/12] Compiling Programming Lab (PL) Stack ^& Queue Tests (CO-3)...
g++ -std=c++11 -I./PL/src PL/src/CombatEventStack.cpp PL/src/EnemySpawnQueue.cpp PL/tests/test_stack_queue.cpp -o bin/test_stack_queue.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL stack and queue tests.
    exit /b 1
)

echo [5/12] Compiling Problem Solving using OOP (PSOOP) Java Module...
javac -d bin/psoop PSOOP/src/exception/*.java PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/ui/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PSOOP Java sources.
    exit /b 1
)

echo [6/12] Compiling Computer Organization ^& Architecture (COA) 64-bit ALU Simulator ^& Assembly (CO-3)...
nasm -f win64 COA\ALP\coa_co3_arithmetic.asm -o COA\ALP\coa_co3_arithmetic.obj
nasm -f win64 COA\ALP\coa_co3_string.asm -o COA\ALP\coa_co3_string.obj
g++ -std=c++11 -I./COA/ALU -I./COA/ALP COA/ALU/ALU80386.cpp COA/ALU/test_alu.cpp COA\ALP\coa_co3_arithmetic.obj COA\ALP\coa_co3_string.obj -o bin/test_alu.exe
g++ -std=c++11 -I./COA/ALU -I./COA/ALP COA/ALU/ALU80386.cpp COA/ALU/test_coa_co3.cpp COA\ALP\coa_co3_arithmetic.obj COA\ALP\coa_co3_string.obj -o bin/test_coa_co3.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile COA ALU tests.
    exit /b 1
)

echo [7/12] Compiling Computer Graphics Lab (CGL) Transformation Tests (CO-2)...
g++ -std=c++11 -I./CGL/src -I./PL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/src/AreaFill.cpp CGL/src/Mesh3D.cpp PL/src/CombatEventStack.cpp PL/src/EnemySpawnQueue.cpp CGL/tests/test_transformations.cpp -o bin/test_transformations.exe -lopengl32 -lglu32 -lglut32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL transformation tests.
    exit /b 1
)

echo [8/12] Compiling Computer Graphics Lab (CGL) Bresenham Line ^& Circle Tests (CO-2)...
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/tests/test_bresenham.cpp -o bin/test_bresenham.exe -lopengl32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL Bresenham algorithm tests.
    exit /b 1
)

echo [9/12] Compiling Computer Graphics Lab (CGL) Scan-Line Fill Tests (CO-3)...
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/tests/test_scanline_fill.cpp -o bin/test_scanline_fill.exe -lopengl32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL scanline fill tests.
    exit /b 1
)

echo [10/12] Compiling Computer Graphics Lab (CGL) Area Fill Tests (Flood-Fill ^& Boundary-Fill) (CO-3)...
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/src/AreaFill.cpp CGL/tests/test_area_fill.cpp -o bin/test_area_fill.exe -lopengl32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL area fill tests.
    exit /b 1
)

echo [11/12] Compiling Computer Graphics Lab (CGL) 3D Mesh Pipeline Tests (CO-3)...
g++ -std=c++11 -I./CGL/src CGL/src/Mesh3D.cpp CGL/tests/test_3d_pipeline.cpp -o bin/test_3d_pipeline.exe -lopengl32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL 3D pipeline tests.
    exit /b 1
)

echo [12/12] Compiling Computer Graphics Lab (CGL) Integrated Desktop Game (CO-3)...
g++ -std=c++11 -I./PL/src -I./COA/ALU -I./COA/ALP -I./CGL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/src/AreaFill.cpp CGL/src/Mesh3D.cpp CGL/src/NetworkBridge.cpp PL/src/BulletPoolArray.cpp PL/src/EnemyLinkedList.cpp PL/src/AsteroidGridMatrix.cpp PL/src/CombatEventStack.cpp PL/src/EnemySpawnQueue.cpp COA/ALU/ALU80386.cpp CGL/src/MainGame.cpp COA\ALP\coa_co3_arithmetic.obj COA\ALP\coa_co3_string.obj -o bin/AirStriker_CO1.exe -lopengl32 -lglu32 -lglut32 -lws2_32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL game executable.
    exit /b 1
)

echo ====================================================================
echo   *** BUILD SUCCESSFUL: ALL SUBJECT TARGETS READY! ***
echo ====================================================================
