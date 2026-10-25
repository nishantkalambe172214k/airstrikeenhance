@echo off
echo ====================================================================
echo    LAUNCHING AIRSTRIKER: JAVA SWING DESKTOP ARCADE SHOOTER          
echo    Module: Problem Solving Using Object Oriented Programming (PSOOP)
echo ====================================================================

if not exist bin\psoop mkdir bin\psoop
javac -d bin/psoop PSOOP/src/exception/*.java PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/ui/*.java PSOOP/src/test/*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile PSOOP Java sources.
    pause
    exit /b 1
)

java -cp bin\psoop ui.SwingGameLauncher
