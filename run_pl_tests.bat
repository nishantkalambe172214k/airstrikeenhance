@echo off
echo ====================================================================
echo   RUNNING PROGRAMMING LAB (PL) DATA STRUCTURE TESTS
echo ====================================================================
if not exist bin\test_bullet_pool.exe (
    call build_all.bat
)
echo --- [1/2] Array Bullet Pool (CO-1) ---
"bin\test_bullet_pool.exe"
echo.
echo --- [2/2] Singly Linked List Enemy Manager (CO-2) ---
"bin\test_enemy_list.exe"

