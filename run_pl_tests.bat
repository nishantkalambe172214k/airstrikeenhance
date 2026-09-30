@echo off
echo ====================================================================
echo   RUNNING PROGRAMMING LAB (PL) DATA STRUCTURE TESTS
echo ====================================================================
if not exist bin\test_bullet_pool.exe (
    call build_all.bat
)
echo --- [1/3] Array Bullet Pool (CO-1) ---
"bin\test_bullet_pool.exe"
echo.
echo --- [2/3] Singly Linked List Enemy Manager (CO-2) ---
"bin\test_enemy_list.exe"
echo.
echo --- [3/3] 2D Array Asteroid Grid Matrix (CO-2) ---
"bin\test_asteroid_grid.exe"
echo.
