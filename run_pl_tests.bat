@echo off
echo ====================================================================
echo   RUNNING PROGRAMMING LAB (PL) DATA STRUCTURE TESTS
echo ====================================================================
if not exist bin\test_bullet_pool.exe (
    call build_all.bat
)
echo --- [1/4] Array Bullet Pool (CO-1) ---
"bin\test_bullet_pool.exe"
echo.
echo --- [2/4] Singly Linked List Enemy Manager (CO-2) ---
"bin\test_enemy_list.exe"
echo.
echo --- [3/4] 2D Array Asteroid Grid Matrix (CO-2) ---
"bin\test_asteroid_grid.exe"
echo.
echo --- [4/4] Stack & Queue Combat History & Wave Spawning (CO-3) ---
"bin\test_stack_queue.exe"
echo.
