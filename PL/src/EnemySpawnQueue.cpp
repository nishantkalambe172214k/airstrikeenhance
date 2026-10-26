#include "EnemySpawnQueue.h"

// ============================================================================
// [PL CO-3] ENEMY SPAWN QUEUE IMPLEMENTATION
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// Genuine FIFO queue managing enemy-wave arrivals in strict first-in, first-out order.
// ============================================================================

// [PL CO-3] Constructor: Initialize empty queue with null front & rear pointers
EnemySpawnQueue::EnemySpawnQueue()
    : frontNode(nullptr), rearNode(nullptr), count(0), nextRequestId(1) {}

// [PL CO-3] Destructor: Deallocate all pending queue nodes
EnemySpawnQueue::~EnemySpawnQueue() {
    clear();
}

// [PL CO-3] Enqueue Operation: Append new spawn request to rear of queue (O(1))
// FIFO rule: New elements join at the rear, oldest stay at the front.
void EnemySpawnQueue::enqueue(const EnemySpawnRequest& request) {
    // 1. Allocate new dynamic node
    QueueNode* newNode = new QueueNode(request);

    // 2. Insert into linked queue
    if (rearNode == nullptr) {
        // [PL CO-3] Special case: Queue was empty, both front and rear point to new node
        frontNode = rearNode = newNode;
    } else {
        // [PL CO-3] General case: Link current rear to newNode, then move rear pointer forward
        rearNode->next = newNode;
        rearNode = newNode;
    }
    count++;
}

// [PL CO-3] Queue Spawn Helper: Construct request and enqueue in one step
void EnemySpawnQueue::queueSpawn(const std::string& type, float x, float y, float vx, float vy,
                                 int health, int maxHealth, int scoreValue, int collisionDamage) {
    EnemySpawnRequest req(nextRequestId++, type, x, y, vx, vy, health, maxHealth, scoreValue, collisionDamage);
    enqueue(req);
}

// [PL CO-3] Dequeue Operation: Remove and return oldest request from front (FIFO, O(1))
// Safe underflow guard: Returns false if queue is empty, avoiding crashes.
bool EnemySpawnQueue::dequeue(EnemySpawnRequest& outRequest) {
    // [PL CO-3] Step 1: Defensive check for empty queue
    if (isEmpty()) {
        return false;
    }

    // [PL CO-3] Step 2: Grab data from frontNode
    QueueNode* temp = frontNode;
    outRequest = temp->data;

    // [PL CO-3] Step 3: Advance frontNode to next node
    frontNode = frontNode->next;

    // [PL CO-3] Step 4: If queue became empty after removal, reset rearNode to null
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    // [PL CO-3] Step 5: Delete old front node to prevent memory leaks
    delete temp;
    count--;
    return true;
}

// [PL CO-3] Dequeue Overload: Discard front request without output parameter
bool EnemySpawnQueue::dequeue() {
    if (isEmpty()) {
        return false; // Safe empty guard
    }

    QueueNode* temp = frontNode;
    frontNode = frontNode->next;
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    delete temp;
    count--;
    return true;
}

// [PL CO-3] Peek/Front Operation: Inspect oldest request at front without dequeuing (O(1))
// Safe check: Returns false if queue is empty
bool EnemySpawnQueue::peek(EnemySpawnRequest& outRequest) const {
    if (isEmpty()) {
        return false; // Safe empty guard
    }
    outRequest = frontNode->data; // Read data directly from front
    return true;
}

// [PL CO-3] Peek Pointer Overload: Returns nullptr if queue is empty
const EnemySpawnRequest* EnemySpawnQueue::peek() const {
    if (isEmpty()) {
        return nullptr;
    }
    return &(frontNode->data);
}

// [PL CO-3] Empty Query: Check if queue has any pending requests (O(1))
bool EnemySpawnQueue::isEmpty() const {
    return frontNode == nullptr;
}

// [PL CO-3] Size Query: Returns total count of pending spawn requests
int EnemySpawnQueue::getSize() const {
    return count;
}

// [PL CO-3] Clear Operation: Deallocate all queue nodes and reset pointers
void EnemySpawnQueue::clear() {
    while (frontNode != nullptr) {
        QueueNode* temp = frontNode;
        frontNode = frontNode->next;
        delete temp;
    }
    rearNode = nullptr;
    count = 0;
}

// [PL CO-3] Traversal Helper: Extract pending requests from front to rear for UI inspector
std::vector<EnemySpawnRequest> EnemySpawnQueue::getPendingRequests(int maxCount) const {
    std::vector<EnemySpawnRequest> list;
    QueueNode* curr = frontNode; // Start at FRONT (next to spawn)
    int fetched = 0;
    while (curr != nullptr && fetched < maxCount) {
        list.push_back(curr->data);
        curr = curr->next; // Move toward REAR
        fetched++;
    }
    return list;
}
