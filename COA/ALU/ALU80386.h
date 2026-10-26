#ifndef ALU_X86_64_H
#define ALU_X86_64_H

// ============================================================================
// [COA CO3] 64-BIT ARITHMETIC LOGIC UNIT (ALU_x86_64) SIMULATOR
// Course Outcome 3: Algorithmic and String Processing
//
// Upgraded from legacy 32-bit 80386 (EAX, EDX) to modern 64-bit x86_64 (RAX, RDX, RCX, RBX).
// Includes 64-bit registers, RFLAGS emulation, hardware binary multiplication
// (Add-and-Shift and Successive Addition), and 64-bit string processing.
// Backward compatibility with 32-bit ALU80386 interface is fully preserved.
// ============================================================================

#include <iostream>
#include <cstdint>
#include <cstddef>
#include <string>

class ALU_x86_64 {
private:
    // [COA CO3] 64-bit General Purpose Registers
    uint64_t rax;   // Accumulator register
    uint64_t rdx;   // Data register
    uint64_t rcx;   // Count register
    uint64_t rbx;   // Base register

    // [COA CO3] 64-bit RFLAGS Status Flags
    bool zeroFlag;      // ZF: Set if result is zero
    bool carryFlag;     // CF: Set if unsigned overflow/borrow occurs
    bool signFlag;      // SF: Set if MSB (bit 63) is set (negative in 2's complement)
    bool overflowFlag;  // OF: Set if signed overflow occurs

public:
    ALU_x86_64();

    // ------------------------------------------------------------------------
    // [COA CO3] 64-Bit Register Accessors
    // ------------------------------------------------------------------------
    void setRAX(uint64_t val);
    void setRDX(uint64_t val);
    void setRCX(uint64_t val);
    void setRBX(uint64_t val);

    uint64_t getRAX() const;
    uint64_t getRDX() const;
    uint64_t getRCX() const;
    uint64_t getRBX() const;

    // ------------------------------------------------------------------------
    // [COA CO3] 32-Bit Backward Compatibility Accessors (Lower 32-bits)
    // In x86_64, writing to a 32-bit subregister (EAX/EDX) zero-extends to 64-bit
    // ------------------------------------------------------------------------
    void setEAX(uint32_t val);
    void setEDX(uint32_t val);
    uint32_t getEAX() const;
    uint32_t getEDX() const;

    // ------------------------------------------------------------------------
    // [COA CO3] RFLAGS Accessors
    // ------------------------------------------------------------------------
    bool getZF() const;
    bool getCF() const;
    bool getSF() const;
    bool getOF() const;

    // ------------------------------------------------------------------------
    // [COA CO3] 64-Bit Core ALU Operations
    // ------------------------------------------------------------------------
    uint64_t executeADD64(bool verbose = false);
    uint64_t executeSUB64(bool verbose = false);

    // ------------------------------------------------------------------------
    // [COA CO3] 32-Bit Compatibility ALU Operations
    // ------------------------------------------------------------------------
    uint32_t executeADD(bool verbose = false);
    uint32_t executeSUB(bool verbose = false);

    // ------------------------------------------------------------------------
    // [COA CO3] Syllabus Arithmetic Multiplication Algorithms
    // ------------------------------------------------------------------------
    /**
     * @brief Computes multiplication using Add-and-Shift binary algorithm.
     * Delegates to 64-bit Assembly (asm_multiply_shift_add) when available.
     */
    uint64_t executeMultiplyShiftAdd(uint64_t multiplicand, uint64_t multiplier, bool verbose = false);

    /**
     * @brief Computes multiplication using Successive Addition loop.
     * Delegates to 64-bit Assembly (asm_multiply_successive) when available.
     */
    uint64_t executeMultiplySuccessive(uint64_t multiplicand, uint64_t multiplier, bool verbose = false);

    /**
     * @brief Game integration: calculates combo score bounty using 64-bit multiplier.
     */
    uint64_t computeScoreMultiplier(uint64_t baseScore, uint64_t comboMultiplier, bool verbose = false);

    // ------------------------------------------------------------------------
    // [COA CO3] Syllabus String Processing Algorithms
    // ------------------------------------------------------------------------
    /**
     * @brief Calculates string length via 64-bit assembly routine.
     */
    uint64_t executeStringLength(const char* str, bool verbose = false);

    /**
     * @brief Converts ASCII string to uppercase in place or into destination buffer.
     */
    uint64_t executeStringToUpper(const char* src, char* dst, size_t maxLen, bool verbose = false);

    /**
     * @brief Lexicographical comparison of two null-terminated strings.
     */
    int64_t executeStringCompare(const char* s1, const char* s2, bool verbose = false);

    // ------------------------------------------------------------------------
    // [COA CO3] Game Gameplay Utility Functions
    // ------------------------------------------------------------------------
    uint32_t computeScoreAdd(uint32_t currentScore, uint32_t increment, bool verbose = false);
    uint32_t computeHealthDamage(uint32_t currentHealth, uint32_t damage, bool& isGameOver, bool verbose = false);
};

// [COA CO3] Backward compatibility alias for existing game code
using ALU80386 = ALU_x86_64;

#endif // ALU_X86_64_H
