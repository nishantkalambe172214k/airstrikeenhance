# AirStriker — Master Build Script (PowerShell)
Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "  BUILDING AIRSTRIKER — 2D SPACE SHOOTER (CO-1 & CO-2)              " -ForegroundColor Green
Write-Host "====================================================================" -ForegroundColor Cyan

if (-not (Test-Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }
if (-not (Test-Path "bin\psoop")) { New-Item -ItemType Directory -Path "bin\psoop" | Out-Null }

Write-Host "[1/8] Compiling Programming Lab (PL) Bullet Pool Tests (CO-1)..." -ForegroundColor Yellow
g++ -std=c++11 -I./PL/src PL/src/BulletPoolArray.cpp PL/tests/test_bullet_pool.cpp -o bin/test_bullet_pool.exe
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile PL bullet pool tests."; exit 1 }

Write-Host "[2/8] Compiling Programming Lab (PL) Enemy Linked List Tests (CO-2)..." -ForegroundColor Yellow
g++ -std=c++11 -I./PL/src PL/src/EnemyLinkedList.cpp PL/tests/test_enemy_list.cpp -o bin/test_enemy_list.exe
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile PL enemy linked list tests."; exit 1 }

Write-Host "[3/8] Compiling Programming Lab (PL) 2D Array Asteroid Grid Matrix Tests (CO-2)..." -ForegroundColor Yellow
g++ -std=c++11 -I./PL/src PL/src/AsteroidGridMatrix.cpp PL/tests/test_asteroid_grid.cpp -o bin/test_asteroid_grid.exe
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile PL asteroid grid matrix tests."; exit 1 }

Write-Host "[4/8] Compiling Problem Solving using OOP (PSOOP) Java Module..." -ForegroundColor Yellow
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile PSOOP Java sources."; exit 1 }

Write-Host "[5/8] Compiling Computer Organization & Architecture (COA) ALU Simulator..." -ForegroundColor Yellow
g++ -std=c++11 -I./COA/ALU COA/ALU/ALU80386.cpp COA/ALU/test_alu.cpp -o bin/test_alu.exe
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile COA ALU tests."; exit 1 }

Write-Host "[6/8] Compiling Computer Graphics Lab (CGL) Transformation Tests (CO-2)..." -ForegroundColor Yellow
g++ -std=c++11 -I./CGL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/tests/test_transformations.cpp -o bin/test_transformations.exe -lopengl32 -lglu32 -lglut32
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile CGL transformation tests."; exit 1 }

Write-Host "[7/8] Compiling Computer Graphics Lab (CGL) Bresenham Line & Circle Tests (CO-2)..." -ForegroundColor Yellow
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/tests/test_bresenham.cpp -o bin/test_bresenham.exe -lopengl32
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile CGL Bresenham algorithm tests."; exit 1 }

Write-Host "[8/8] Compiling Computer Graphics Lab (CGL) Integrated Desktop Game (CO-2)..." -ForegroundColor Yellow
g++ -std=c++11 -I./PL/src -I./COA/ALU -I./CGL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp PL/src/BulletPoolArray.cpp PL/src/EnemyLinkedList.cpp PL/src/AsteroidGridMatrix.cpp COA/ALU/ALU80386.cpp CGL/src/MainGame.cpp -o bin/AirStriker_CO1.exe -lopengl32 -lglu32 -lglut32
if ($LASTEXITCODE -ne 0) { Write-Error "Failed to compile CGL game executable."; exit 1 }

# Ensure glut32.dll is located next to the executable
if (Test-Path "C:\MinGW\bin\glut32.dll") {
    Copy-Item "C:\MinGW\bin\glut32.dll" -Destination "bin\glut32.dll" -Force
}

Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "  *** BUILD SUCCESSFUL: ALL SUBJECT TARGETS READY! ***              " -ForegroundColor Green
Write-Host "====================================================================" -ForegroundColor Cyan
