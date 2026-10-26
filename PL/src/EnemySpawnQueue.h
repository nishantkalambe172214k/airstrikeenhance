#ifndef ENEMY_SPAWN_QUEUE_H
#define ENEMY_SPAWN_QUEUE_H

#include <string>
#include <vector>

// ============================================================================
// [PL CO-3] ENEMY SPAWN REQUEST DATA STRUCTURE
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// Stores pending spawn parameters processed in strict FIFO arrival order.
// ============================================================================
struct EnemySpawnRequest {
    int requestId;
    std::string enemyType; // "DRONE", "FIGHTER", "BOSS"
    float spawnX;
    float spawnY;
    float vx;
    float vy;
    int health;
    int maxHealth;
    int scoreValue;
    int collisionDamage;

    EnemySpawnRequest()
        : requestId(0), enemyType("DRONE"), spawnX(0.0f), spawnY(0.0f),
          vx(0.0f), vy(0.0f), health(40), maxHealth(40),
          scoreValue(100), collisionDamage(25) {}

    EnemySpawnRequest(int id, const std::string& type, float x, float y,
                      float velX, float velY, int hp, int maxHp, int score, int dmg)
        : requestId(id), enemyType(type), spawnX(x), spawnY(y),
          vx(velX), vy(velY), health(hp), maxHealth(maxHp),
          scoreValue(score), collisionDamage(dmg) {}
};

// [PL CO-3] Dynamic Queue Node for Linked Queue implementation
struct QueueNode {
    EnemySpawnRequest data;
    QueueNode* next;

    QueueNode(const EnemySpawnRequest& req)
        : data(req), next(nullptr) {}
};

// ============================================================================
// [PL CO-3] ENEMY SPAWN QUEUE CLASS (FIFO: First-In, First-Out)
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// - Stores enemy spawn requests where front of queue is the first to be spawned.
// - Demonstrates O(1) enqueue, dequeue, peek/front, and isEmpty operations.
// - Defensive bounds checking safely handles empty-queue operations without crash.
// ============================================================================
class EnemySpawnQueue {
private:
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;
    int nextRequestId;

public:
    EnemySpawnQueue();
    ~EnemySpawnQueue();

    // Prevent accidental copying / double-free
    EnemySpawnQueue(const EnemySpawnQueue&) = delete;
    EnemySpawnQueue& operator=(const EnemySpawnQueue&) = delete;

    // [PL CO-3] Enqueue Operation: Appends request to the rear of the queue (O(1))
    void enqueue(const EnemySpawnRequest& request);

    // [PL CO-3] Convenience helper to construct and enqueue a spawn request
    void queueSpawn(const std::string& type, float x, float y, float vx, float vy,
                    int health, int maxHealth, int scoreValue, int collisionDamage);

    // [PL CO-3] Dequeue Operation: Removes and returns the front request (FIFO). Returns false if empty.
    bool dequeue(EnemySpawnRequest& outRequest);

    // [PL CO-3] Dequeue overload: Discards the front request without returning it
    bool dequeue();

    // [PL CO-3] Peek/Front Operation: Inspects front request without removal. Returns false if empty.
    bool peek(EnemySpawnRequest& outRequest) const;

    // [PL CO-3] Peek pointer overload: Returns nullptr if empty
    const EnemySpawnRequest* peek() const;

    // [PL CO-3] Status queries
    bool isEmpty() const;
    int getSize() const;
    void clear();

    // [PL CO-3] Helper for UI inspector: Extracts pending requests from front to rear
    std::vector<EnemySpawnRequest> getPendingRequests(int maxCount = 10) const;
};

#endif // ENEMY_SPAWN_QUEUE_H
