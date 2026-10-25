#include "NetworkBridge.h"
#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/select.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

NetworkBridge::NetworkBridge() : socketFd(INVALID_SOCKET), connected(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

NetworkBridge::~NetworkBridge() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool NetworkBridge::setNonBlocking(bool nonBlocking) {
    if (socketFd == INVALID_SOCKET) return false;
#ifdef _WIN32
    u_long mode = nonBlocking ? 1 : 0;
    return ioctlsocket(socketFd, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(socketFd, F_GETFL, 0);
    if (flags < 0) return false;
    flags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return fcntl(socketFd, F_SETFL, flags) == 0;
#endif
}

bool NetworkBridge::connectTo(const std::string& host, int port) {
    disconnect();

    socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketFd == INVALID_SOCKET) {
        return false;
    }

    // Disable Nagle algorithm for immediate frame message delivery
    int nodelay = 1;
    setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay));

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        closesocket(socketFd);
        socketFd = INVALID_SOCKET;
        return false;
    }

    if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        closesocket(socketFd);
        socketFd = INVALID_SOCKET;
        return false;
    }

    connected = true;
    setNonBlocking(true);
    recvBuffer.clear();
    std::cout << ">>> [NETWORK BRIDGE] Connected to PSOOP Java Engine on " << host << ":" << port << " <<<\n";
    return true;
}

void NetworkBridge::disconnect() {
    if (socketFd != INVALID_SOCKET) {
        closesocket(socketFd);
        socketFd = INVALID_SOCKET;
    }
    connected = false;
    recvBuffer.clear();
}

bool NetworkBridge::sendLine(const std::string& line) {
    if (!connected || socketFd == INVALID_SOCKET) return false;

    std::string payload = line + "\n";
    int sent = send(socketFd, payload.c_str(), static_cast<int>(payload.length()), 0);
    if (sent <= 0) {
        disconnect();
        return false;
    }
    return true;
}

bool NetworkBridge::receiveLine(std::string& outLine, int timeoutMs) {
    if (!connected || socketFd == INVALID_SOCKET) return false;

    // Check if we already have a full line in buffer
    size_t newlinePos = recvBuffer.find('\n');
    if (newlinePos != std::string::npos) {
        outLine = recvBuffer.substr(0, newlinePos);
        recvBuffer.erase(0, newlinePos + 1);
        if (!outLine.empty() && outLine.back() == '\r') {
            outLine.pop_back();
        }
        return true;
    }

    // Wait with select() for data
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(socketFd, &readFds);

    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int ready = select(socketFd + 1, &readFds, nullptr, nullptr, &tv);
    if (ready <= 0) {
        return false; // Timeout or error
    }

    char buffer[2048];
    int bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        disconnect();
        return false;
    }

    buffer[bytesRead] = '\0';
    recvBuffer.append(buffer, bytesRead);

    newlinePos = recvBuffer.find('\n');
    if (newlinePos != std::string::npos) {
        outLine = recvBuffer.substr(0, newlinePos);
        recvBuffer.erase(0, newlinePos + 1);
        if (!outLine.empty() && outLine.back() == '\r') {
            outLine.pop_back();
        }
        return true;
    }

    return false;
}

bool NetworkBridge::ping() {
    if (!sendLine("PING")) return false;
    std::string resp;
    if (receiveLine(resp, 50)) {
        return resp.find("PONG") != std::string::npos;
    }
    return false;
}

bool NetworkBridge::initGame(const std::string& pilot) {
    if (!sendLine("INIT " + pilot)) return false;
    std::string resp;
    return receiveLine(resp, 50);
}

bool NetworkBridge::spawnWave(int waveNum, std::vector<RemoteEnemy>& outEnemies) {
    if (!sendLine("SPAWN_WAVE " + std::to_string(waveNum))) return false;
    std::string resp;
    if (receiveLine(resp, 50)) {
        if (resp.find("SPAWN_OK") != std::string::npos) {
            parseEnemiesString(resp, outEnemies);
            return true;
        }
    }
    return false;
}

bool NetworkBridge::tickSimulation(float dx, float dy, RemoteGameState& outState, std::vector<RemoteEnemy>& outEnemies) {
    if (!sendLine("TICK")) return false;
    std::string resp;
    if (receiveLine(resp, 10)) {
        if (resp.find("TICK_OK") != std::string::npos) {
            const std::string delimiter = "##ENEMIES##";
            size_t delimPos = resp.find(delimiter);
            std::string statePart = (delimPos != std::string::npos) ? resp.substr(8, delimPos - 8) : resp.substr(8);
            parseStateString(statePart, outState);

            if (delimPos != std::string::npos) {
                std::string enemiesPart = resp.substr(delimPos + delimiter.length());
                parseEnemiesString(enemiesPart, outEnemies);
            }
            return true;
        }
    }
    return false;
}

bool NetworkBridge::sendAluFeedback(int damage, int bounty, uint32_t eax, uint32_t edx, int zf, int sf, int cf) {
    std::ostringstream oss;
    oss << "ALU_FEEDBACK " << damage << " " << bounty << " " << eax << " " << edx << " " << zf << " " << sf << " " << cf;
    if (!sendLine(oss.str())) return false;
    std::string resp;
    return receiveLine(resp, 20);
}

bool NetworkBridge::resetGame() {
    if (!sendLine("RESET")) return false;
    std::string resp;
    return receiveLine(resp, 50);
}

void NetworkBridge::parseStateString(const std::string& str, RemoteGameState& state) {
    // Expected format: STATE px=100.00 py=300.00 hp=100 maxHp=100 sh=50 maxSh=50 sc=0 wv=1 over=false hud="SCORE: 0 | ..."
    size_t pos;
    if ((pos = str.find("px=")) != std::string::npos) state.playerX = std::stof(str.substr(pos + 3));
    if ((pos = str.find("py=")) != std::string::npos) state.playerY = std::stof(str.substr(pos + 3));
    if ((pos = str.find("hp=")) != std::string::npos) state.health = std::stoi(str.substr(pos + 3));
    if ((pos = str.find("maxHp=")) != std::string::npos) state.maxHealth = std::stoi(str.substr(pos + 6));
    if ((pos = str.find("sh=")) != std::string::npos) state.shield = std::stoi(str.substr(pos + 3));
    if ((pos = str.find("maxSh=")) != std::string::npos) state.maxShield = std::stoi(str.substr(pos + 6));
    if ((pos = str.find("sc=")) != std::string::npos) state.score = std::stoi(str.substr(pos + 3));
    if ((pos = str.find("wv=")) != std::string::npos) state.wave = std::stoi(str.substr(pos + 3));
    if ((pos = str.find("over=")) != std::string::npos) state.isGameOver = (str.substr(pos + 5, 4) == "true");

    size_t hudStart = str.find("hud=\"");
    if (hudStart != std::string::npos) {
        size_t hudEnd = str.find("\"", hudStart + 5);
        if (hudEnd != std::string::npos) {
            state.hudText = str.substr(hudStart + 5, hudEnd - (hudStart + 5));
        }
    }
}

void NetworkBridge::parseEnemiesString(const std::string& str, std::vector<RemoteEnemy>& enemies) {
    // Format: ENEMIES count=3;id,type,x,y,hp,maxHp,sc,dmg;id,...
    enemies.clear();
    size_t firstSemicolon = str.find(';');
    if (firstSemicolon == std::string::npos) return;

    std::string listStr = str.substr(firstSemicolon + 1);
    std::istringstream stream(listStr);
    std::string enemyToken;

    while (std::getline(stream, enemyToken, ';')) {
        if (enemyToken.empty()) continue;
        std::istringstream tokenStream(enemyToken);
        std::string part;
        RemoteEnemy e;
        int fieldIdx = 0;

        while (std::getline(tokenStream, part, ',')) {
            switch (fieldIdx) {
                case 0: e.id = std::stoi(part); break;
                case 1: e.type = part; break;
                case 2: e.x = std::stof(part); break;
                case 3: e.y = std::stof(part); break;
                case 4: e.health = std::stoi(part); break;
                case 5: e.maxHealth = std::stoi(part); break;
                case 6: e.scoreValue = std::stoi(part); break;
                case 7: e.collisionDamage = std::stoi(part); break;
            }
            fieldIdx++;
        }
        if (fieldIdx >= 6) {
            enemies.push_back(e);
        }
    }
}
