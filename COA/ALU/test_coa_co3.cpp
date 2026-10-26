// ============================================================================
// [COA CO3] COMPREHENSIVE UNIT TEST SUITE FOR 64-BIT ALU & ASSEMBLY ROUTINES
// Course Outcome 3: Algorithmic and String Processing
//
// Tests:
// 1. 32-bit Backward Compatibility (Legacy ADD/SUB score & health checks)
// 2. 64-bit Native ALU (ADD64, SUB64, RFLAGS ZF/CF/SF/OF)
// 3. 64-bit Arithmetic Multiplication (Shift-and-Add vs Successive vs Reference)
// 4. 64-bit String Processing (String Length, Uppercase Conversion, String Compare)
// ============================================================================

#include <iostream>
#include <cassert>
#include <cstring>
#include "ALU80386.h"
#include "../ALP/COA_CO3_Assembly.h"

// ----------------------------------------------------------------------------
// [COA CO3] TEST 1: Backward Compatibility (Legacy 32-bit operations)
// ----------------------------------------------------------------------------
void test32BitCompatibility() {
    std::cout << "[COA CO3 TEST 1] Verifying 32-bit Backward Compatibility...\n";
    ALU_x86_64 alu;

    // Test ADD
    uint32_t res1 = alu.computeScoreAdd(500, 150, false);
    assert(res1 == 650);
    assert(!alu.getCF() && !alu.getZF() && !alu.getSF());

    alu.setEAX(0xFFFFFFFFU);
    alu.setEDX(1);
    uint32_t res2 = alu.executeADD(false);
    assert(res2 == 0);
    assert(alu.getCF() == true && alu.getZF() == true);

    // Test SUB
    bool isGameOver = false;
    uint32_t hp1 = alu.computeHealthDamage(100, 25, isGameOver, false);
    assert(hp1 == 75);
    assert(!isGameOver);

    uint32_t hp2 = alu.computeHealthDamage(50, 50, isGameOver, false);
    assert(hp2 == 0);
    assert(isGameOver == true && alu.getZF() == true);

    std::cout << " -> PASSED: 32-bit compatibility verified.\n";
}

// ----------------------------------------------------------------------------
// [COA CO3] TEST 2: 64-bit Core ALU ADD64 and SUB64 with RFLAGS
// ----------------------------------------------------------------------------
void test64BitCoreALU() {
    std::cout << "\n[COA CO3 TEST 2] Verifying 64-bit Core ALU (ADD64 / SUB64 / RFLAGS)...\n";
    ALU_x86_64 alu;

    // Standard 64-bit addition
    alu.setRAX(0x100000000ULL);
    alu.setRDX(0x200000000ULL);
    uint64_t sum1 = alu.executeADD64(true);
    assert(sum1 == 0x300000000ULL);
    assert(!alu.getZF() && !alu.getCF() && !alu.getSF());

    // 64-bit Carry Flag (unsigned overflow at 2^64 - 1)
    alu.setRAX(0xFFFFFFFFFFFFFFFFULL);
    alu.setRDX(1ULL);
    uint64_t sum2 = alu.executeADD64(true);
    assert(sum2 == 0);
    assert(alu.getCF() == true); // Carry flag set
    assert(alu.getZF() == true); // Zero flag set

    // 64-bit Subtraction with borrow (Carry Flag)
    alu.setRAX(50ULL);
    alu.setRDX(100ULL);
    uint64_t diff1 = alu.executeSUB64(true);
    assert(alu.getCF() == true); // Borrow occurred
    assert(alu.getSF() == true); // Result is negative in two's complement

    // 64-bit Subtraction resulting in zero
    alu.setRAX(0xDEADBEEFCAFEULL);
    alu.setRDX(0xDEADBEEFCAFEULL);
    uint64_t diff2 = alu.executeSUB64(true);
    assert(diff2 == 0);
    assert(alu.getZF() == true);
    assert(!alu.getCF());

    std::cout << " -> PASSED: 64-bit ALU operations and RFLAGS verified.\n";
}

// ----------------------------------------------------------------------------
// [COA CO3] TEST 3: 64-bit Arithmetic Multiplication (Shift-and-Add & Successive)
// ----------------------------------------------------------------------------
void test64BitMultiplicationAlgorithms() {
    std::cout << "\n[COA CO3 TEST 3] Verifying 64-bit Multiplication Algorithms...\n";
    ALU_x86_64 alu;

    struct TestCase {
        uint64_t m;
        uint64_t q;
        uint64_t expected;
    };

    TestCase cases[] = {
        {0, 0, 0},
        {0, 500, 0},
        {500, 0, 0},
        {1, 1, 1},
        {1, 999999ULL, 999999ULL},
        {999999ULL, 1, 999999ULL},
        {250, 4, 1000},
        {12345, 678, 8369910ULL},
        {1500, 5, 7500},            // Wave combo score multiplier scenario
        {1000000ULL, 2000000ULL, 2000000000000ULL}
    };

    for (const auto& tc : cases) {
        // 1. Test native 64-bit NASM Add-and-Shift
        uint64_t prod_shift_add = asm_multiply_shift_add(tc.m, tc.q);
        assert(prod_shift_add == tc.expected);

        // 2. Test native 64-bit NASM Successive Addition (for moderate q)
        if (tc.q <= 10000) {
            uint64_t prod_succ = asm_multiply_successive(tc.m, tc.q);
            assert(prod_succ == tc.expected);
        }

        // 3. Test C++ reference implementations
        uint64_t ref_shift = COA_Reference::ref_multiply_shift_add(tc.m, tc.q);
        assert(ref_shift == tc.expected);

        // 4. Test ALU wrapper method
        uint64_t alu_mult = alu.executeMultiplyShiftAdd(tc.m, tc.q, false);
        assert(alu_mult == tc.expected);
    }

    // Verify game score combo multiplier method
    uint64_t comboBounty = alu.computeScoreMultiplier(250, 3, true);
    assert(comboBounty == 750);

    std::cout << " -> PASSED: Shift-and-Add and Successive Addition algorithms verified across all test cases.\n";
}

// ----------------------------------------------------------------------------
// [COA CO3] TEST 4: 64-bit String Processing Algorithms
// ----------------------------------------------------------------------------
void test64BitStringProcessing() {
    std::cout << "\n[COA CO3 TEST 4] Verifying 64-bit String Processing Algorithms...\n";
    ALU_x86_64 alu;

    // --- Sub-test 4A: String Length ---
    std::cout << "  [4A] Testing asm_str_length...\n";
    assert(asm_str_length("") == 0);
    assert(asm_str_length(nullptr) == 0);
    assert(asm_str_length("A") == 1);
    assert(asm_str_length("AirStriker-64Bit") == 16);
    assert(alu.executeStringLength("Viper_Squadron", true) == 14);

    // --- Sub-test 4B: String Uppercase Conversion ---
    std::cout << "  [4B] Testing asm_str_to_upper...\n";
    char buffer[64];

    // Standard string conversion
    uint64_t count1 = asm_str_to_upper("pilot_alpha_01", buffer, sizeof(buffer));
    assert(count1 == 14);
    assert(std::strcmp(buffer, "PILOT_ALPHA_01") == 0);

    // Mixed alphanumeric and punctuation
    uint64_t count2 = alu.executeStringToUpper("callsign: ghost-9!", buffer, sizeof(buffer), true);
    assert(count2 == 18);
    assert(std::strcmp(buffer, "CALLSIGN: GHOST-9!") == 0);

    // Buffer limit clamping (max_len = 5 allows 4 chars + '\0')
    char smallBuf[5];
    uint64_t count3 = asm_str_to_upper("superlongcallsign", smallBuf, sizeof(smallBuf));
    assert(count3 == 4);
    assert(std::strcmp(smallBuf, "SUPE") == 0);

    // Zero and 1-byte capacity edge cases
    char oneBuf[1];
    uint64_t count4 = asm_str_to_upper("test", oneBuf, 1);
    assert(count4 == 0);
    assert(oneBuf[0] == '\0');

    // --- Sub-test 4C: String Lexicographical Comparison ---
    std::cout << "  [4C] Testing asm_str_compare...\n";
    assert(asm_str_compare("EAGLE", "EAGLE") == 0);
    assert(alu.executeStringCompare("ALPHA", "ALPHA", true) == 0);

    assert(asm_str_compare("ALPHA", "BRAVO") < 0);
    assert(alu.executeStringCompare("ALPHA", "BRAVO", true) < 0);

    assert(asm_str_compare("ZULU", "YANKEE") > 0);
    assert(alu.executeStringCompare("ZULU", "YANKEE", true) > 0);

    assert(asm_str_compare("AIR", "AIRSTRIKER") < 0);
    assert(asm_str_compare("AIRSTRIKER", "AIR") > 0);

    // Null pointer handling
    assert(asm_str_compare(nullptr, nullptr) == 0);
    assert(asm_str_compare(nullptr, "NON_NULL") < 0);
    assert(asm_str_compare("NON_NULL", nullptr) > 0);

    std::cout << " -> PASSED: String length, uppercase, and comparison algorithms verified.\n";
}

// ----------------------------------------------------------------------------
// Main Entry Point
// ----------------------------------------------------------------------------
int main() {
    std::cout << "=================================================================\n";
    std::cout << "  AIR STRIKER: COA CO3 64-BIT ALU & ASSEMBLY TEST SUITE          \n";
    std::cout << "  Course Outcome 3: Algorithmic and String Processing            \n";
    std::cout << "=================================================================\n";

    test32BitCompatibility();
    test64BitCoreALU();
    test64BitMultiplicationAlgorithms();
    test64BitStringProcessing();

    std::cout << "\n=================================================================\n";
    std::cout << "  >>> ALL COA CO3 64-BIT ALU & ASSEMBLY TESTS PASSED (100%) <<<  \n";
    std::cout << "=================================================================\n";
    return 0;
}
