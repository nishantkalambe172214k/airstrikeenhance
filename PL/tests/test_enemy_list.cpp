#include <iostream>
#include <cassert>
#include "../src/EnemyLinkedList.h"

void testAddEnemiesAndTraversal() {
    std::cout << "[TEST 1] Adding Enemies and Traversing Singly Linked List...\n";
    EnemyLinkedList list;
    assert(list.isEmpty());
    assert(list.getCount() == 0);

    list.addEnemy(101, 700.0f, 200.0f, -2.0f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(102, 750.0f, 300.0f, -2.5f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(103, 800.0f, 400.0f, -3.0f, 0.0f, 40, 40, 100, 20);

    assert(list.getCount() == 3);
    assert(!list.isEmpty());

    int ids[3] = { 103, 102, 101 };
    int idx = 0;
    EnemyNode* curr = list.getHead();
    while (curr != nullptr) {
        assert(curr->data.id == ids[idx]);
        std::cout << " -> Node [" << idx << "] Enemy ID=" << curr->data.id
                  << " Pos=(" << curr->data.x << ", " << curr->data.y << ")\n";
        curr = curr->next;
        idx++;
    }
    assert(idx == 3);
    std::cout << " -> Singly linked list nodes created and traversed successfully.\n";
}

void testRemoveEnemyHead() {
    std::cout << "\n[TEST 2] Removing Head Enemy Node...\n";
    EnemyLinkedList list;
    list.addEnemy(201, 600.0f, 100.0f, -2.0f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(202, 650.0f, 200.0f, -2.0f, 0.0f, 40, 40, 100, 20);

    assert(list.getCount() == 2);
    assert(list.getHead()->data.id == 202);

    bool removed = list.removeEnemy(202);
    assert(removed == true);
    assert(list.getCount() == 1);
    assert(list.getHead()->data.id == 201);
    std::cout << " -> Head node removed and head pointer updated cleanly.\n";
}

void testRemoveEnemyMiddleAndTail() {
    std::cout << "\n[TEST 3] Removing Middle and Tail Nodes...\n";
    EnemyLinkedList list;
    list.addEnemy(301, 500.0f, 100.0f, -2.0f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(302, 550.0f, 200.0f, -2.0f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(303, 600.0f, 300.0f, -2.0f, 0.0f, 40, 40, 100, 20);

    bool removedMid = list.removeEnemy(302);
    assert(removedMid == true);
    assert(list.getCount() == 2);

    EnemyNode* curr = list.getHead();
    assert(curr->data.id == 303);
    assert(curr->next->data.id == 301);
    assert(curr->next->next == nullptr);

    bool removedTail = list.removeEnemy(301);
    assert(removedTail == true);
    assert(list.getCount() == 1);
    assert(list.getHead()->data.id == 303);
    assert(list.getHead()->next == nullptr);
    std::cout << " -> Middle and tail nodes unlinked and deleted properly.\n";
}

void testClearList() {
    std::cout << "\n[TEST 4] Clearing Entire Linked List...\n";
    EnemyLinkedList list;
    list.addEnemy(401, 500.0f, 100.0f, -2.0f, 0.0f, 40, 40, 100, 20);
    list.addEnemy(402, 550.0f, 200.0f, -2.0f, 0.0f, 40, 40, 100, 20);

    list.clear();
    assert(list.isEmpty());
    assert(list.getCount() == 0);
    assert(list.getHead() == nullptr);
    std::cout << " -> All nodes cleared and memory reclaimed.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: PL CO-2 TEST SUITE (EnemyLinkedList)  \n";
    std::cout << "=====================================================\n\n";

    testAddEnemiesAndTraversal();
    testRemoveEnemyHead();
    testRemoveEnemyMiddleAndTail();
    testClearList();

    std::cout << "\n>>> ALL PL CO-2 ENEMY LINKED LIST TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
