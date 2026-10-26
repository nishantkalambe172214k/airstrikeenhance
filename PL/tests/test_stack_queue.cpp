#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../src/CombatEventStack.h"
#include "../src/EnemySpawnQueue.h"

// ============================================================================
// [PL CO-3] TEST SUITE: STACK AND QUEUE DATA STRUCTURES
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// Verifies genuine LIFO (Stack) and FIFO (Queue) behaviors, safe empty bounds,
// and scenario-based combat/spawning integration.
// ============================================================================

void testStackLIFOOrder() {
    std::cout << "[TEST 1] Testing CombatEventStack LIFO (Last-In, First-Out) Ordering...\n";
    CombatEventStack stack;
    assert(stack.isEmpty());
    assert(stack.getSize() == 0);

    // [PL CO-3] Push 3 distinct combat events
    stack.recordEvent(CombatEventType::ENEMY_DESTROYED, "Destroyed Drone #101", 100, 1.25f);
    stack.recordEvent(CombatEventType::ASTEROID_DESTROYED, "Pulverized Asteroid (2,3)", 50, 2.10f);
    stack.recordEvent(CombatEventType::HULL_DAMAGE, "Hull Impact with Obstacle", 25, 3.45f);

    assert(!stack.isEmpty());
    assert(stack.getSize() == 3);

    // [PL CO-3] Peek: top of stack must be the MOST RECENT event (HULL_DAMAGE)
    CombatEvent topEvt;
    assert(stack.peek(topEvt));
    assert(topEvt.type == CombatEventType::HULL_DAMAGE);
    assert(topEvt.value == 25);
    std::cout << " -> Top event verified (LIFO): " << topEvt.description << " [Value=" << topEvt.value << "]\n";

    // [PL CO-3] Pop 1: must be HULL_DAMAGE (last pushed)
    CombatEvent popped;
    assert(stack.pop(popped));
    assert(popped.type == CombatEventType::HULL_DAMAGE);
    assert(stack.getSize() == 2);
    std::cout << " -> Popped #1: " << popped.description << "\n";

    // [PL CO-3] Pop 2: must be ASTEROID_DESTROYED
    assert(stack.pop(popped));
    assert(popped.type == CombatEventType::ASTEROID_DESTROYED);
    assert(stack.getSize() == 1);
    std::cout << " -> Popped #2: " << popped.description << "\n";

    // [PL CO-3] Pop 3: must be ENEMY_DESTROYED (first pushed)
    assert(stack.pop(popped));
    assert(popped.type == CombatEventType::ENEMY_DESTROYED);
    assert(stack.getSize() == 0);
    assert(stack.isEmpty());
    std::cout << " -> Popped #3: " << popped.description << "\n";

    std::cout << " -> PASS: CombatEventStack strictly adheres to LIFO semantics.\n";
}

void testStackSafeEmptyOperations() {
    std::cout << "\n[TEST 2] Testing CombatEventStack Safe Empty-Stack Handling...\n";
    CombatEventStack stack;

    // [PL CO-3] Pop on empty stack should return false safely, never throw or crash
    CombatEvent evt;
    bool popResult = stack.pop(evt);
    assert(!popResult);
    assert(!stack.pop()); // overload
    assert(!stack.peek(evt));
    assert(stack.peek() == nullptr);
    assert(stack.isEmpty());
    assert(stack.getSize() == 0);

    // [PL CO-3] Push items, clear, and verify clean reset
    stack.recordEvent(CombatEventType::POWERUP_COLLECTED, "Shield Recharge", 100, 5.0f);
    stack.recordEvent(CombatEventType::WAVE_CLEARED, "Wave 1 Cleared", 500, 10.0f);
    assert(stack.getSize() == 2);

    stack.clear();
    assert(stack.isEmpty());
    assert(stack.getSize() == 0);
    assert(!stack.pop(evt));

    std::cout << " -> PASS: Defensive guards safely handle empty-stack access without crashing.\n";
}

void testQueueFIFOOrder() {
    std::cout << "\n[TEST 3] Testing EnemySpawnQueue FIFO (First-In, First-Out) Ordering...\n";
    EnemySpawnQueue queue;
    assert(queue.isEmpty());
    assert(queue.getSize() == 0);

    // [PL CO-3] Enqueue 3 enemy spawn requests in wave arrival order
    queue.queueSpawn("DRONE", 820.0f, 150.0f, -2.5f, 0.0f, 30, 30, 100, 15);
    queue.queueSpawn("FIGHTER", 860.0f, 300.0f, -3.5f, 0.0f, 60, 60, 250, 30);
    queue.queueSpawn("BOSS", 900.0f, 450.0f, -1.5f, 0.0f, 200, 200, 1000, 50);

    assert(!queue.isEmpty());
    assert(queue.getSize() == 3);

    // [PL CO-3] Peek: front of queue must be the FIRST request enqueued (DRONE)
    EnemySpawnRequest frontReq;
    assert(queue.peek(frontReq));
    assert(frontReq.enemyType == "DRONE");
    assert(frontReq.requestId == 1);
    std::cout << " -> Front request verified (FIFO): " << frontReq.enemyType << " at Y=" << frontReq.spawnY << "\n";

    // [PL CO-3] Dequeue 1: must be DRONE (oldest request)
    EnemySpawnRequest dequeued;
    assert(queue.dequeue(dequeued));
    assert(dequeued.enemyType == "DRONE");
    assert(dequeued.requestId == 1);
    assert(queue.getSize() == 2);
    std::cout << " -> Dequeued #1: " << dequeued.enemyType << " (ID=" << dequeued.requestId << ")\n";

    // [PL CO-3] Dequeue 2: must be FIGHTER
    assert(queue.dequeue(dequeued));
    assert(dequeued.enemyType == "FIGHTER");
    assert(dequeued.requestId == 2);
    assert(queue.getSize() == 1);
    std::cout << " -> Dequeued #2: " << dequeued.enemyType << " (ID=" << dequeued.requestId << ")\n";

    // [PL CO-3] Dequeue 3: must be BOSS (last enqueued)
    assert(queue.dequeue(dequeued));
    assert(dequeued.enemyType == "BOSS");
    assert(dequeued.requestId == 3);
    assert(queue.getSize() == 0);
    assert(queue.isEmpty());
    std::cout << " -> Dequeued #3: " << dequeued.enemyType << " (ID=" << dequeued.requestId << ")\n";

    std::cout << " -> PASS: EnemySpawnQueue strictly adheres to FIFO semantics.\n";
}

void testQueueSafeEmptyOperations() {
    std::cout << "\n[TEST 4] Testing EnemySpawnQueue Safe Empty-Queue Handling...\n";
    EnemySpawnQueue queue;

    // [PL CO-3] Dequeue on empty queue should return false safely
    EnemySpawnRequest req;
    bool deqResult = queue.dequeue(req);
    assert(!deqResult);
    assert(!queue.dequeue()); // overload
    assert(!queue.peek(req));
    assert(queue.peek() == nullptr);
    assert(queue.isEmpty());
    assert(queue.getSize() == 0);

    // [PL CO-3] Enqueue items, clear, and verify reset
    queue.queueSpawn("DRONE", 800.0f, 200.0f, -2.0f, 0.0f, 30, 30, 100, 15);
    queue.queueSpawn("DRONE", 850.0f, 350.0f, -2.0f, 0.0f, 30, 30, 100, 15);
    assert(queue.getSize() == 2);

    queue.clear();
    assert(queue.isEmpty());
    assert(queue.getSize() == 0);
    assert(!queue.dequeue(req));

    std::cout << " -> PASS: Defensive guards safely handle empty-queue access without crashing.\n";
}

void testScenarioIntegration() {
    std::cout << "\n[TEST 5] Testing Integrated Combat Scenario (Simultaneous Stack & Queue)...\n";
    CombatEventStack combatHistory;
    EnemySpawnQueue waveQueue;

    // [PL CO-3] Scenario Step 1: Queue a wave of 4 enemy fighters (FIFO)
    for (int i = 0; i < 4; ++i) {
        waveQueue.queueSpawn("FIGHTER", 800.0f + i * 40.0f, 150.0f + i * 80.0f,
                             -3.0f, 0.0f, 50, 50, 150, 20);
    }
    assert(waveQueue.getSize() == 4);

    // [PL CO-3] Scenario Step 2: Spawn first 2 enemies in FIFO order
    EnemySpawnRequest spawned1, spawned2;
    assert(waveQueue.dequeue(spawned1));
    assert(waveQueue.dequeue(spawned2));
    assert(spawned1.requestId == 1);
    assert(spawned2.requestId == 2);
    assert(waveQueue.getSize() == 2);

    // [PL CO-3] Scenario Step 3: Record player destroying enemy 1, then taking hit from enemy 2 (LIFO)
    combatHistory.recordEvent(CombatEventType::ENEMY_DESTROYED, "Neutralized Enemy #1", 150, 1.5f);
    combatHistory.recordEvent(CombatEventType::HULL_DAMAGE, "Direct Hit by Enemy #2", 20, 1.8f);
    assert(combatHistory.getSize() == 2);

    // [PL CO-3] Scenario Step 4: Top of stack is most recent combat occurrence (HULL_DAMAGE)
    CombatEvent latestCombat;
    assert(combatHistory.peek(latestCombat));
    assert(latestCombat.type == CombatEventType::HULL_DAMAGE);

    // [PL CO-3] Scenario Step 5: Next wave unit to deploy is still strictly FIFO (Enemy #3)
    EnemySpawnRequest nextToSpawn;
    assert(waveQueue.peek(nextToSpawn));
    assert(nextToSpawn.requestId == 3);

    // [PL CO-3] Verify recent history inspector
    std::vector<CombatEvent> recent = combatHistory.getRecentEvents(5);
    assert(recent.size() == 2);
    assert(recent[0].type == CombatEventType::HULL_DAMAGE);      // Top (most recent)
    assert(recent[1].type == CombatEventType::ENEMY_DESTROYED);  // Preceding event

    std::vector<EnemySpawnRequest> pending = waveQueue.getPendingRequests(5);
    assert(pending.size() == 2);
    assert(pending[0].requestId == 3); // Front (next to spawn)
    assert(pending[1].requestId == 4); // Rear

    std::cout << " -> Recent Combat Events (LIFO Top-to-Bottom):\n";
    for (const auto& ev : recent) {
        std::cout << "    * [" << ev.eventId << "] " << ev.description << "\n";
    }
    std::cout << " -> Pending Wave Spawns (FIFO Front-to-Rear):\n";
    for (const auto& rq : pending) {
        std::cout << "    * [" << rq.requestId << "] " << rq.enemyType << " at (" << rq.spawnX << ", " << rq.spawnY << ")\n";
    }

    std::cout << " -> PASS: Scenario successfully demonstrated Stack (LIFO) and Queue (FIFO) synergy.\n";
}

int main() {
    std::cout << "========================================================\n";
    std::cout << " AIR STRIKER - PROGRAMMING LABORATORY (PL) CO-3 TESTS\n";
    std::cout << " Course Outcome 3: Stack (LIFO) and Queue (FIFO) (L3)\n";
    std::cout << "========================================================\n\n";

    testStackLIFOOrder();
    testStackSafeEmptyOperations();
    testQueueFIFOOrder();
    testQueueSafeEmptyOperations();
    testScenarioIntegration();

    std::cout << "\n========================================================\n";
    std::cout << " ALL PL CO-3 TESTS PASSED SUCCESSFULLY! (5/5)\n";
    std::cout << "========================================================\n";
    return 0;
}
