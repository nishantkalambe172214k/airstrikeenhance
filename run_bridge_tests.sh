#!/bin/bash
echo "===================================================================="
echo "   AIR STRIKER: 4-SUBJECT REAL-TIME BRIDGE VERIFICATION            "
echo "===================================================================="

# Compile Java
javac -d bin/psoop PSOOP/src/model/*.java PSOOP/src/service/*.java PSOOP/src/test/*.java
if [ $? -ne 0 ]; then
    echo "[ERROR] Java compilation failed."
    exit 1
fi

# Compile Bridge Test
g++ -std=c++11 -I./CGL/src CGL/src/NetworkBridge.cpp CGL/tests/test_bridge_integration.cpp -o bin/test_bridge_integration
if [ $? -ne 0 ]; then
    echo "[ERROR] C++ test compilation failed."
    exit 1
fi

# Start server
java -cp bin/psoop service.PSOOPBridgeServer &
SERVER_PID=$!
sleep 1

# Run test
./bin/test_bridge_integration
TEST_RES=$?

# Kill server
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

exit $TEST_RES
