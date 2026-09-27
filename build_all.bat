@echo off
echo ====================================================================
echo   BUILDING AIRSTRIKER — 2D SPACE SHOOTER
echo ====================================================================

if not exist bin mkdir bin
if not exist bin\psoop mkdir bin\psoop

echo [1/5] Compiling Programming Lab (PL) Bullet Pool Tests (CO-1)...
g++ -std=c++11 -I./PL/src PL/src/BulletPoolArray.cpp PL/tests/test_bullet_pool.cpp -o bin/test_bullet_pool.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL bullet pool tests.
    exit /b 1
)

echo [2/5] Compiling Programming Lab (PL) Enemy Linked List Tests (CO-2)...
g++ -std=c++11 -I./PL/src PL/src/EnemyLinkedList.cpp PL/tests/test_enemy_list.cpp -o bin/test_enemy_list.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PL enemy linked list tests.
    exit /b 1
)

echo [3/5] Compiling Problem Solving using OOP (PSOOP) Java Module...
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PSOOP Java sources.
    exit /b 1
)

echo [4/5] Compiling Computer Organization ^& Architecture (COA) ALU Simulator...
g++ -std=c++11 -I./COA/ALU COA/ALU/ALU80386.cpp COA/ALU/test_alu.cpp -o bin/test_alu.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile COA ALU tests.
    exit /b 1
)

echo [5/6] Compiling Computer Graphics Lab (CGL) Transformation Tests (CO-2)...
g++ -std=c++11 -I./CGL/src CGL/src/Renderer.cpp CGL/tests/test_transformations.cpp -o bin/test_transformations.exe -lopengl32 -lglu32 -lglut32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL transformation tests.
    exit /b 1
)

echo [6/6] Compiling Computer Graphics Lab (CGL) Integrated Desktop Game...
g++ -std=c++11 -I./PL/src -I./COA/ALU -I./CGL/src CGL/src/Renderer.cpp PL/src/BulletPoolArray.cpp PL/src/EnemyLinkedList.cpp COA/ALU/ALU80386.cpp CGL/src/MainGame.cpp -o bin/AirStriker_CO1.exe -lopengl32 -lglu32 -lglut32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile CGL game executable.
    exit /b 1
)

echo ====================================================================
echo   *** BUILD SUCCESSFUL: ALL SUBJECT TARGETS READY! ***
echo ====================================================================
