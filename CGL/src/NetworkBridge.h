#ifndef NETWORK_BRIDGE_H
#define NETWORK_BRIDGE_H

#include <string>
#include <vector>
#include <cstdint>

// Representation of enemy received from PSOOP polymorphic domain model
struct RemoteEnemy {
    int id;
    std::string type; // DRONE, FIGHTER, BOSS
    float x;
    float y;
    int health;
    int maxHealth;
    int scoreValue;
    int collisionDamage;
};

// Representation of full synchronized state from PSOOP GameEngine
struct RemoteGameState {
    float playerX;
    float playerY;
    int health;
    int maxHealth;
    int shield;
    int maxShield;
    int score;
    int wave;
    bool isGameOver;
    std::string hudText;
};

/**
 * Cross-Platform High-Speed IPC Network Bridge
 * Connects CGL (OpenGL), PL (Data Structures), and COA (ALU) to PSOOP (Java)
 */
class NetworkBridge {
private:
    int socketFd;
    bool connected;
    std::string recvBuffer;

    bool setNonBlocking(bool nonBlocking);

public:
    NetworkBridge();
    ~NetworkBridge();

    bool connectTo(const std::string& host = "127.0.0.1", int port = 9092);
    void disconnect();
    bool isConnected() const { return connected; }

    bool sendLine(const std::string& line);
    bool receiveLine(std::string& outLine, int timeoutMs = 5);

    // High-Level Domain Synchronizations with PSOOP
    bool ping();
    bool initGame(const std::string& pilot = "CadetPilot");
    bool spawnWave(int waveNum, std::vector<RemoteEnemy>& outEnemies);
    bool tickSimulation(float dx, float dy, RemoteGameState& outState, std::vector<RemoteEnemy>& outEnemies);
    bool sendAluFeedback(int damage, int bounty, uint32_t eax, uint32_t edx, int zf, int sf, int cf);
    bool resetGame();

    static void parseStateString(const std::string& str, RemoteGameState& state);
    static void parseEnemiesString(const std::string& str, std::vector<RemoteEnemy>& enemies);
};

#endif // NETWORK_BRIDGE_H
