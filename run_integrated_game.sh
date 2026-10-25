#!/bin/bash
echo "===================================================================="
echo "   AIRSTRIKER: 4-SUBJECT REAL-TIME INTEGRATED LAUNCHER              "
echo "   PL (C++) + COA (80386 ALU) + CGL (OpenGL) + PSOOP (Java Engine)  "
echo "===================================================================="

# Compile PSOOP Java classes
echo "[1/3] Building PSOOP Java Subsystem..."
javac -d bin/psoop PSOOP/src/exception/*.java PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/ui/*.java PSOOP/src/test/*.java
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile Java classes."
    exit 1
fi

# Compile CGL Integrated Game
echo "[2/3] Building Native C++ Engine (CGL + PL + COA + NetworkBridge)..."
g++ -std=c++11 -I./PL/src -I./COA/ALU -I./CGL/src \
    CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/src/AreaFill.cpp CGL/src/Mesh3D.cpp CGL/src/NetworkBridge.cpp \
    PL/src/BulletPoolArray.cpp PL/src/EnemyLinkedList.cpp PL/src/AsteroidGridMatrix.cpp \
    COA/ALU/ALU80386.cpp CGL/src/MainGame.cpp \
    -o bin/AirStriker -framework OpenGL -framework GLUT
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile native C++ executable."
    exit 1
fi

# Start Java PSOOP Bridge Server in background
echo "[3/3] Starting PSOOP Java Domain Server on 127.0.0.1:9092..."
java -cp bin/psoop service.PSOOPBridgeServer &
JAVA_PID=$!
sleep 1

# Trap EXIT to cleanly kill the background Java server
cleanup() {
    echo ""
    echo "[SHUTDOWN] Terminating PSOOP Java Domain Server (PID $JAVA_PID)..."
    kill $JAVA_PID 2>/dev/null
    echo "[SHUTDOWN] Mission Terminated Cleanly."
}
trap cleanup EXIT INT TERM

echo ">>> Launching 4-Subject Integrated Desktop Game Window <<<"
./bin/AirStriker
