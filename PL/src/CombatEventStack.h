#ifndef COMBAT_EVENT_STACK_H
#define COMBAT_EVENT_STACK_H

#include <string>
#include <vector>

// ============================================================================
// [PL CO-3] COMBAT EVENT DATA STRUCTURE
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// Represents a discrete combat occurrence stored in the LIFO CombatEventStack.
// ============================================================================
enum class CombatEventType {
    ENEMY_DESTROYED,     // Enemy fighter/drone destroyed by player weapon
    ASTEROID_DESTROYED,  // Matrix asteroid pulverized by laser fire
    SHIELD_ABSORB,       // Energy shield absorbed impact/damage
    HULL_DAMAGE,         // Direct hull integrity loss from collision
    POWERUP_COLLECTED,   // Auxiliary repair or energy core collected
    WAVE_CLEARED         // All hostile entities in wave neutralized
};

// [PL CO-3] Record stored on the Stack representing combat history
struct CombatEvent {
    int eventId;
    CombatEventType type;
    std::string description;
    int value;         // Score bounty, damage absorbed, or wave number
    float timestamp;   // Game simulation timestamp in seconds

    CombatEvent()
        : eventId(0), type(CombatEventType::ENEMY_DESTROYED), description(""), value(0), timestamp(0.0f) {}

    CombatEvent(int id, CombatEventType t, const std::string& desc, int val, float time)
        : eventId(id), type(t), description(desc), value(val), timestamp(time) {}
};

// [PL CO-3] Dynamic Stack Node for Linked Stack implementation
struct StackNode {
    CombatEvent data;
    StackNode* next;

    StackNode(const CombatEvent& evt, StackNode* n = nullptr)
        : data(evt), next(n) {}
};

// ============================================================================
// [PL CO-3] COMBAT EVENT STACK CLASS (LIFO: Last-In, First-Out)
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// - Stores combat history where top of stack is always the most recent event.
// - Demonstrates O(1) push, pop, peek/top, and isEmpty operations.
// - Defensive bounds checking safely handles empty-stack operations without crash.
// ============================================================================
class CombatEventStack {
private:
    StackNode* topNode;
    int count;
    int nextEventId;

public:
    CombatEventStack();
    ~CombatEventStack();

    // Prevent accidental copying / double-free
    CombatEventStack(const CombatEventStack&) = delete;
    CombatEventStack& operator=(const CombatEventStack&) = delete;

    // [PL CO-3] Push: Adds an event to the top of the stack (O(1))
    void push(const CombatEvent& event);

    // [PL CO-3] Convenience helper to construct and push an event
    void recordEvent(CombatEventType type, const std::string& desc, int value, float timestamp);

    // [PL CO-3] Pop: Removes and returns the top event (LIFO). Returns false if empty.
    bool pop(CombatEvent& outEvent);

    // [PL CO-3] Pop overload: Discards the top event without returning it
    bool pop();

    // [PL CO-3] Peek: Inspects the top event without removal. Returns false if empty.
    bool peek(CombatEvent& outEvent) const;

    // [PL CO-3] Peek pointer overload: Returns nullptr if empty
    const CombatEvent* peek() const;

    // [PL CO-3] Status queries
    bool isEmpty() const;
    int getSize() const;
    void clear();

    // [PL CO-3] Helper for UI inspector: Extracts recent events from top to bottom
    std::vector<CombatEvent> getRecentEvents(int maxCount = 10) const;
};

#endif // COMBAT_EVENT_STACK_H
