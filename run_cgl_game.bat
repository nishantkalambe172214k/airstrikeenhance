@echo off
echo ====================================================================
echo   LAUNCHING AIRSTRIKER — 2D SPACE SHOOTER (CO-1 PLAYABLE GAME)
echo ====================================================================
if not exist bin\AirStriker_CO1.exe (
    echo Executable not found. Compiling first...
    call build_all.bat
)
start "" "bin\AirStriker_CO1.exe"
