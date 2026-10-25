#!/bin/bash
echo "===================================================================="
echo "   AIR STRIKER: PSOOP MASTER ACADEMIC TEST SUITE (Java OOP)         "
echo "   CO-1 & CO-2 Fundamentals + Exception Handling, Collections,     "
echo "   Packages, and Java Swing Desktop GUI Architecture                "
echo "===================================================================="

mkdir -p bin/psoop
javac -d bin/psoop PSOOP/src/exception/*.java PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/ui/*.java PSOOP/src/test/*.java
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile PSOOP Java sources."
    exit 1
fi

java -cp bin/psoop test.PSOOPTestRunner
exit $?
