#include "CombatEventStack.h"
#include <algorithm>

// ============================================================================
// [PL CO-3] COMBAT EVENT STACK IMPLEMENTATION
// Course Outcome 3: Build scenario-based applications using Stack and Queue (L3).
// Genuine LIFO stack managing in-game combat telemetry history.
// ============================================================================

// [PL CO-3] Constructor: Initialize empty stack with null top pointer and zero count
CombatEventStack::CombatEventStack()
    : topNode(nullptr), count(0), nextEventId(1) {}

// [PL CO-3] Destructor: Release all dynamically allocated nodes
CombatEventStack::~CombatEventStack() {
    clear();
}

// [PL CO-3] Push Operation: Insert new combat event onto top of stack (O(1))
// LIFO rule: Newest event becomes the new topNode.
void CombatEventStack::push(const CombatEvent& event) {
    // 1. Allocate new node pointing to current top
    StackNode* newNode = new StackNode(event, topNode);
    // 2. Update topNode pointer to the new node
    topNode = newNode;
    // 3. Increment active element counter
    count++;
}

// [PL CO-3] Record Event Helper: Auto-generates ID and pushes event in one call
void CombatEventStack::recordEvent(CombatEventType type, const std::string& desc, int value, float timestamp) {
    CombatEvent evt(nextEventId++, type, desc, value, timestamp);
    push(evt);
}

// [PL CO-3] Pop Operation: Remove and return the most recent event (LIFO, O(1))
// Safe underflow guard: Returns false if stack is empty, avoiding crashes.
bool CombatEventStack::pop(CombatEvent& outEvent) {
    // [PL CO-3] Step 1: Defensive check for empty stack
    if (isEmpty()) {
        return false;
    }

    // [PL CO-3] Step 2: Grab data from topNode
    StackNode* temp = topNode;
    outEvent = temp->data;

    // [PL CO-3] Step 3: Advance topNode to the next node in the chain
    topNode = topNode->next;

    // [PL CO-3] Step 4: Delete the old top node to prevent memory leaks
    delete temp;
    count--;
    return true;
}

// [PL CO-3] Pop Overload: Discard top event without output parameter
bool CombatEventStack::pop() {
    if (isEmpty()) {
        return false; // Safe empty check
    }
    StackNode* temp = topNode;
    topNode = topNode->next;
    delete temp;
    count--;
    return true;
}

// [PL CO-3] Peek/Top Operation: Inspect top event without removal (O(1))
// Safe check: Returns false if empty
bool CombatEventStack::peek(CombatEvent& outEvent) const {
    if (isEmpty()) {
        return false; // Safe empty guard
    }
    outEvent = topNode->data; // Read data directly from top
    return true;
}

// [PL CO-3] Peek Pointer Overload: Returns nullptr if stack is empty
const CombatEvent* CombatEventStack::peek() const {
    if (isEmpty()) {
        return nullptr;
    }
    return &(topNode->data);
}

// [PL CO-3] Empty Query: Check if stack has any elements (O(1))
bool CombatEventStack::isEmpty() const {
    return topNode == nullptr;
}

// [PL CO-3] Size Query: Returns total count of elements currently on stack
int CombatEventStack::getSize() const {
    return count;
}

// [PL CO-3] Clear Operation: Deallocate all linked nodes and reset state
void CombatEventStack::clear() {
    while (topNode != nullptr) {
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    count = 0;
}

// [PL CO-3] History Traversal: Extract top N events (most recent first) for UI inspection
std::vector<CombatEvent> CombatEventStack::getRecentEvents(int maxCount) const {
    std::vector<CombatEvent> list;
    StackNode* curr = topNode; // Start at TOP (most recent)
    int fetched = 0;
    while (curr != nullptr && fetched < maxCount) {
        list.push_back(curr->data);
        curr = curr->next; // Move down the stack
        fetched++;
    }
    return list;
}
