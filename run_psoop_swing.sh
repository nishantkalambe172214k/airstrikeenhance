#!/bin/bash
echo "===================================================================="
echo "   LAUNCHING AIRSTRIKER: JAVA SWING DESKTOP ARCADE SHOOTER          "
echo "   Module: Problem Solving Using Object Oriented Programming (PSOOP)"
echo "===================================================================="

mkdir -p bin/psoop
javac -d bin/psoop PSOOP/src/exception/*.java PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/ui/*.java PSOOP/src/test/*.java
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile PSOOP Java sources."
    exit 1
fi

java -cp bin/psoop ui.SwingGameLauncher
