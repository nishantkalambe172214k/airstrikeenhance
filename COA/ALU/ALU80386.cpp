#include "ALU80386.h"
#include "../ALP/COA_CO3_Assembly.h"
#include <iostream>
#include <iomanip>

// ============================================================================
// [COA CO3] 64-BIT ALU (ALU_x86_64) IMPLEMENTATION
// Course Outcome 3: Algorithmic and String Processing
// ============================================================================

ALU_x86_64::ALU_x86_64()
    : rax(0), rdx(0), rcx(0), rbx(0),
      zeroFlag(false), carryFlag(false), signFlag(false), overflowFlag(false) {}

// ----------------------------------------------------------------------------
// [COA CO3] 64-Bit Register Accessors
// ----------------------------------------------------------------------------
void ALU_x86_64::setRAX(uint64_t val) { rax = val; }
void ALU_x86_64::setRDX(uint64_t val) { rdx = val; }
void ALU_x86_64::setRCX(uint64_t val) { rcx = val; }
void ALU_x86_64::setRBX(uint64_t val) { rbx = val; }

uint64_t ALU_x86_64::getRAX() const { return rax; }
uint64_t ALU_x86_64::getRDX() const { return rdx; }
uint64_t ALU_x86_64::getRCX() const { return rcx; }
uint64_t ALU_x86_64::getRBX() const { return rbx; }

// ----------------------------------------------------------------------------
// [COA CO3] 32-Bit Backward Compatibility Accessors (Zero-Extended on write)
// ----------------------------------------------------------------------------
void ALU_x86_64::setEAX(uint32_t val) {
    // In x86_64 architecture, writing to 32-bit EAX clears the upper 32 bits of RAX
    rax = static_cast<uint64_t>(val);
}

void ALU_x86_64::setEDX(uint32_t val) {
    // In x86_64 architecture, writing to 32-bit EDX clears the upper 32 bits of RDX
    rdx = static_cast<uint64_t>(val);
}

uint32_t ALU_x86_64::getEAX() const {
    return static_cast<uint32_t>(rax & 0xFFFFFFFFULL);
}

uint32_t ALU_x86_64::getEDX() const {
    return static_cast<uint32_t>(rdx & 0xFFFFFFFFULL);
}

// ----------------------------------------------------------------------------
// [COA CO3] RFLAGS Status Register Accessors
// ----------------------------------------------------------------------------
bool ALU_x86_64::getZF() const { return zeroFlag; }
bool ALU_x86_64::getCF() const { return carryFlag; }
bool ALU_x86_64::getSF() const { return signFlag; }
bool ALU_x86_64::getOF() const { return overflowFlag; }

// ----------------------------------------------------------------------------
// [COA CO3] 64-Bit Core ALU Operations
// ----------------------------------------------------------------------------
uint64_t ALU_x86_64::executeADD64(bool verbose) {
    uint64_t opA = rax;
    uint64_t opB = rdx;
    uint64_t sum = opA + opB;

    // [COA CO3] 64-bit Flag Calculations
    carryFlag    = (sum < opA);                         // Unsigned wrap-around
    zeroFlag     = (sum == 0);                          // All 64 bits zero
    signFlag     = ((sum & (1ULL << 63)) != 0);         // Bit 63 set
    overflowFlag = (((opA ^ sum) & (opB ^ sum) & (1ULL << 63)) != 0); // Signed overflow

    rax = sum;

    if (verbose) {
        std::cout << "[ALU64 ADD] 0x" << std::hex << opA << " + 0x" << opB
                  << " = 0x" << rax << std::dec
                  << " (ZF=" << zeroFlag << ", CF=" << carryFlag
                  << ", SF=" << signFlag << ", OF=" << overflowFlag << ")\n";
    }
    return rax;
}

uint64_t ALU_x86_64::executeSUB64(bool verbose) {
    uint64_t opA = rax;
    uint64_t opB = rdx;
    uint64_t diff = opA - opB;

    // [COA CO3] 64-bit Flag Calculations
    carryFlag    = (opA < opB);                         // Unsigned borrow
    zeroFlag     = (diff == 0);                         // All 64 bits zero
    signFlag     = ((diff & (1ULL << 63)) != 0);        // Bit 63 set
    overflowFlag = (((opA ^ opB) & (opA ^ diff) & (1ULL << 63)) != 0); // Signed overflow

    rax = diff;

    if (verbose) {
        std::cout << "[ALU64 SUB] 0x" << std::hex << opA << " - 0x" << opB
                  << " = 0x" << rax << std::dec
                  << " (ZF=" << zeroFlag << ", CF=" << carryFlag
                  << ", SF=" << signFlag << ", OF=" << overflowFlag << ")\n";
    }
    return rax;
}

// ----------------------------------------------------------------------------
// [COA CO3] 32-Bit Compatibility ALU Operations
// Preserves existing 32-bit game behavior and unit test assertions
// ----------------------------------------------------------------------------
uint32_t ALU_x86_64::executeADD(bool verbose) {
    uint32_t opA = getEAX();
    uint32_t opB = getEDX();
    uint64_t sum64 = static_cast<uint64_t>(opA) + static_cast<uint64_t>(opB);

    carryFlag    = (sum64 > 0xFFFFFFFFULL);
    setEAX(static_cast<uint32_t>(sum64));
    zeroFlag     = (getEAX() == 0);
    signFlag     = ((getEAX() & 0x80000000U) != 0);
    overflowFlag = (((opA ^ getEAX()) & (opB ^ getEAX()) & 0x80000000U) != 0);

    if (verbose) {
        std::cout << "[ALU32 ADD] " << opA << " + " << opB << " = " << getEAX()
                  << " (ZF=" << zeroFlag << ", CF=" << carryFlag << ")\n";
    }
    return getEAX();
}

uint32_t ALU_x86_64::executeSUB(bool verbose) {
    uint32_t opA = getEAX();
    uint32_t opB = getEDX();

    carryFlag = (opA < opB);
    zeroFlag  = (opA == opB);
    signFlag  = (opA < opB);

    uint32_t res = (opA >= opB) ? (opA - opB) : 0;
    setEAX(res);

    if (verbose) {
        std::cout << "[ALU32 SUB] " << opA << " - " << opB << " = " << getEAX()
                  << " (ZF=" << zeroFlag << ", SF=" << signFlag << ")\n";
    }
    return getEAX();
}

// ----------------------------------------------------------------------------
// [COA CO3] Syllabus Arithmetic Multiplication Algorithms
// ----------------------------------------------------------------------------
uint64_t ALU_x86_64::executeMultiplyShiftAdd(uint64_t multiplicand, uint64_t multiplier, bool verbose) {
    // [COA CO3] Execute hardware-accurate Add-and-Shift binary algorithm in 64-bit Assembly
    uint64_t prod = asm_multiply_shift_add(multiplicand, multiplier);
    rax = prod;
    rdx = 0;
    zeroFlag = (rax == 0);
    carryFlag = false;
    signFlag = ((rax & (1ULL << 63)) != 0);

    if (verbose) {
        std::cout << "[ALU64 SHIFT-ADD MULT] " << multiplicand << " * " << multiplier
                  << " = " << rax << " (ZF=" << zeroFlag << ")\n";
    }
    return rax;
}

uint64_t ALU_x86_64::executeMultiplySuccessive(uint64_t multiplicand, uint64_t multiplier, bool verbose) {
    // [COA CO3] Execute Successive Addition loop multiplication in 64-bit Assembly
    uint64_t prod = asm_multiply_successive(multiplicand, multiplier);
    rax = prod;
    rcx = 0;
    zeroFlag = (rax == 0);
    carryFlag = false;
    signFlag = ((rax & (1ULL << 63)) != 0);

    if (verbose) {
        std::cout << "[ALU64 SUCCESSIVE MULT] " << multiplicand << " * " << multiplier
                  << " = " << rax << " (ZF=" << zeroFlag << ")\n";
    }
    return rax;
}

uint64_t ALU_x86_64::computeScoreMultiplier(uint64_t baseScore, uint64_t comboMultiplier, bool verbose) {
    // [COA CO3] Gameplay calculation: Wave combo score calculation using 64-bit binary shift-add
    return executeMultiplyShiftAdd(baseScore, comboMultiplier, verbose);
}

// ----------------------------------------------------------------------------
// [COA CO3] Syllabus String Processing Algorithms
// ----------------------------------------------------------------------------
uint64_t ALU_x86_64::executeStringLength(const char* str, bool verbose) {
    // [COA CO3] Call 64-bit Assembly routine
    uint64_t len = asm_str_length(str);
    rcx = len;
    zeroFlag = (len == 0);

    if (verbose) {
        std::cout << "[ALU64 STR LEN] \"" << (str ? str : "(null)") << "\" -> " << len << " bytes\n";
    }
    return len;
}

uint64_t ALU_x86_64::executeStringToUpper(const char* src, char* dst, size_t maxLen, bool verbose) {
    // [COA CO3] Call 64-bit Assembly routine
    uint64_t count = asm_str_to_upper(src, dst, static_cast<uint64_t>(maxLen));
    rax = count;

    if (verbose) {
        std::cout << "[ALU64 STR UPPER] \"" << (src ? src : "(null)") << "\" -> \""
                  << (dst ? dst : "(null)") << "\" (" << count << " chars)\n";
    }
    return count;
}

int64_t ALU_x86_64::executeStringCompare(const char* s1, const char* s2, bool verbose) {
    // [COA CO3] Call 64-bit Assembly routine
    int64_t diff = asm_str_compare(s1, s2);
    rax = static_cast<uint64_t>(diff);
    zeroFlag = (diff == 0);
    signFlag = (diff < 0);

    if (verbose) {
        std::cout << "[ALU64 STR CMP] \"" << (s1 ? s1 : "(null)") << "\" vs \""
                  << (s2 ? s2 : "(null)") << "\" -> " << diff
                  << " (ZF=" << zeroFlag << ", SF=" << signFlag << ")\n";
    }
    return diff;
}

// ----------------------------------------------------------------------------
// [COA CO3] Gameplay Utility Functions
// ----------------------------------------------------------------------------
uint32_t ALU_x86_64::computeScoreAdd(uint32_t currentScore, uint32_t increment, bool verbose) {
    setEAX(currentScore);
    setEDX(increment);
    return executeADD(verbose);
}

uint32_t ALU_x86_64::computeHealthDamage(uint32_t currentHealth, uint32_t damage, bool &isGameOver, bool verbose) {
    setEAX(currentHealth);
    setEDX(damage);
    uint32_t remaining = executeSUB(verbose);

    if (zeroFlag || signFlag || carryFlag) {
        isGameOver = true;
        remaining = 0;
    } else {
        isGameOver = false;
    }
    return remaining;
}
