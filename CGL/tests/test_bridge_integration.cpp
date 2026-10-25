#include <iostream>
#include <cassert>
#include "../src/NetworkBridge.h"

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: 4-SUBJECT NETWORK BRIDGE TEST         \n";
    std::cout << "=====================================================\n";

    NetworkBridge bridge;
    std::cout << "[TEST 1] Connecting to PSOOP Java Server on 127.0.0.1:9092...\n";

    if (!bridge.connectTo("127.0.0.1", 9092)) {
        std::cerr << "[FAIL] Could not connect to PSOOP Java Bridge Server.\n";
        std::cerr << "Ensure PSOOPBridgeServer is running: java -cp bin/psoop service.PSOOPBridgeServer\n";
        return 1;
    }
    std::cout << " -> Connected successfully.\n";

    std::cout << "[TEST 2] Pinging PSOOP Java Engine...\n";
    bool pong = bridge.ping();
    assert(pong && "Ping must return true");
    std::cout << " -> Ping-Pong verified: PSOOP Java Engine responded!\n";

    std::cout << "[TEST 3] Initializing Game with Pilot 'AceViper'...\n";
    bool inited = bridge.initGame("AceViper");
    assert(inited && "Init must return true");
    std::cout << " -> Init acknowledged.\n";

    std::cout << "[TEST 4] Spawning Polymorphic Enemy Wave (Wave 2)...\n";
    std::vector<RemoteEnemy> enemies;
    bool spawned = bridge.spawnWave(2, enemies);
    assert(spawned && "SpawnWave must succeed");
    std::cout << " -> Spawned " << enemies.size() << " enemies from PSOOP Java OOP hierarchy:\n";
    for (const auto& e : enemies) {
        std::cout << "    * Enemy #" << e.id << " [" << e.type << "] Pos=(" << e.x << ", " << e.y 
                  << ") HP=" << e.health << " Dmg=" << e.collisionDamage << "\n";
    }

    std::cout << "[TEST 5] Ticking Simulation (Moving Player & Enemies)...\n";
    RemoteGameState state;
    enemies.clear();
    bool ticked = bridge.tickSimulation(5.0f, 0.0f, state, enemies);
    assert(ticked && "Tick must succeed");
    std::cout << " -> Received State: HP=" << state.health << " Shield=" << state.shield 
              << " Score=" << state.score << " HUD: " << state.hudText << "\n";

    std::cout << "[TEST 6] Transmitting COA 80386 ALU Execution (Damage & EFLAGS)...\n";
    // Simulate COA SUB EAX, EDX (Health = 100 - 20 = 80, ZF=0, SF=0)
    bool aluSynced = bridge.sendAluFeedback(20, 150, 80, 20, 0, 0, 0);
    assert(aluSynced && "ALU feedback must succeed");
    std::cout << " -> COA ALU feedback synchronized with PSOOP domain model!\n";

    bridge.disconnect();
    std::cout << "\n>>> ALL 4-SUBJECT INTER-MODULE BRIDGE TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
