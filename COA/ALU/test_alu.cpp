#include <iostream>
#include <cassert>
#include "ALU80386.h"

void testScoreAddition() {
    std::cout << "[COA TEST 1] ALU: Score Addition (ADD EAX, EDX)...\n";
    ALU80386 alu;

    uint32_t res1 = alu.computeScoreAdd(500, 150, true);
    assert(res1 == 650);
    assert(!alu.getCF());
    assert(!alu.getZF());
    assert(!alu.getSF());

    uint32_t res2 = alu.computeScoreAdd(0, 100, false);
    assert(res2 == 100);
    assert(!alu.getZF());

    alu.setEAX(0);
    alu.setEDX(0);
    uint32_t res3 = alu.executeADD(false);
    assert(res3 == 0);
    assert(alu.getZF() == true);

    alu.setEAX(0xFFFFFFFF);
    alu.setEDX(1);
    uint32_t res4 = alu.executeADD(true);
    assert(res4 == 0);
    assert(alu.getCF() == true);
    assert(alu.getZF() == true);
    std::cout << " -> Score addition and flags verified.\n";
}

void testHealthDamageSubtraction() {
    std::cout << "\n[COA TEST 2] ALU: Health Subtraction (SUB EAX, EDX)...\n";
    ALU80386 alu;
    bool isGameOver = false;

    uint32_t hp1 = alu.computeHealthDamage(100, 25, isGameOver, true);
    assert(hp1 == 75);
    assert(!isGameOver);
    assert(!alu.getZF());
    assert(!alu.getSF());
    assert(!alu.getCF());

    uint32_t hp2 = alu.computeHealthDamage(75, 75, isGameOver, true);
    assert(hp2 == 0);
    assert(isGameOver == true);
    assert(alu.getZF() == true);

    uint32_t hp3 = alu.computeHealthDamage(20, 50, isGameOver, true);
    assert(hp3 == 0);
    assert(isGameOver == true);
    assert(alu.getSF() == true || alu.getCF() == true);
    std::cout << " -> Health damage and game-over condition flags verified.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: COA TEST SUITE (ALU Operations)       \n";
    std::cout << "=====================================================\n";

    testScoreAddition();
    testHealthDamageSubtraction();

    std::cout << "\n>>> ALL COA ALU ARITHMETIC TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
