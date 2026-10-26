#ifndef COA_CO3_ASSEMBLY_H
#define COA_CO3_ASSEMBLY_H

// ============================================================================
// [COA CO3] 64-BIT ASSEMBLY AND ALGORITHMIC INTERFACE
// Course Outcome 3: Algorithmic and String Processing
//
// This header declares the native 64-bit x86_64 Assembly routines assembled via NASM
// and exposes software reference models for hardware algorithm verification.
// ============================================================================

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cctype>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// [COA CO3] 64-Bit Arithmetic Multiplication Algorithms
// ----------------------------------------------------------------------------

/**
 * @brief Add-and-Shift (Shift-and-Add) binary hardware multiplication in 64-bit Assembly.
 * @param multiplicand 64-bit value to be multiplied (shifted left)
 * @param multiplier   64-bit value multiplier (tested and shifted right)
 * @return 64-bit product (RAX)
 */
uint64_t asm_multiply_shift_add(uint64_t multiplicand, uint64_t multiplier);

/**
 * @brief Successive Addition loop multiplication in 64-bit Assembly.
 * @param multiplicand 64-bit value to be added repeatedly
 * @param multiplier   64-bit loop counter
 * @return 64-bit product (RAX)
 */
uint64_t asm_multiply_successive(uint64_t multiplicand, uint64_t multiplier);

// ----------------------------------------------------------------------------
// [COA CO3] 64-Bit String Processing Algorithms
// ----------------------------------------------------------------------------

/**
 * @brief Computes length of null-terminated string using 64-bit pointer traversal.
 * @param str Pointer to null-terminated ASCII string
 * @return String length in bytes (excluding '\0')
 */
uint64_t asm_str_length(const char* str);

/**
 * @brief Converts ASCII string to uppercase in 64-bit Assembly with bounds checking.
 * @param src Pointer to source ASCII string
 * @param dst Pointer to destination buffer
 * @param max_len Total buffer size of dst (including space for '\0')
 * @return Count of characters written into dst
 */
uint64_t asm_str_to_upper(const char* src, char* dst, uint64_t max_len);

/**
 * @brief Lexicographical comparison of two null-terminated strings in 64-bit Assembly.
 * @param s1 Pointer to first string
 * @param s2 Pointer to second string
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int64_t asm_str_compare(const char* s1, const char* s2);

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
// [COA CO3] High-Level C++ Architectural Reference Implementations
// Useful for unit testing, parity checking, and software simulation
// ----------------------------------------------------------------------------
namespace COA_Reference {

    // [COA CO3] Reference Add-and-Shift binary multiplication
    inline uint64_t ref_multiply_shift_add(uint64_t m, uint64_t q) {
        uint64_t acc = 0;
        while (q > 0) {
            if (q & 1) {
                acc += m;
            }
            m <<= 1;
            q >>= 1;
        }
        return acc;
    }

    // [COA CO3] Reference Successive Addition multiplication
    inline uint64_t ref_multiply_successive(uint64_t m, uint64_t q) {
        uint64_t acc = 0;
        for (uint64_t i = 0; i < q; ++i) {
            acc += m;
        }
        return acc;
    }

    // [COA CO3] Reference String Length
    inline uint64_t ref_str_length(const char* str) {
        if (!str) return 0;
        uint64_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        return len;
    }

    // [COA CO3] Reference String Uppercase
    inline uint64_t ref_str_to_upper(const char* src, char* dst, uint64_t max_len) {
        if (!src || !dst || max_len == 0) return 0;
        if (max_len == 1) {
            dst[0] = '\0';
            return 0;
        }
        uint64_t i = 0;
        for (; i < max_len - 1 && src[i] != '\0'; ++i) {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst[i] = static_cast<char>(std::toupper(c));
        }
        dst[i] = '\0';
        return i;
    }

    // [COA CO3] Reference String Compare
    inline int64_t ref_str_compare(const char* s1, const char* s2) {
        if (s1 == s2) return 0;
        if (!s1) return -1;
        if (!s2) return 1;
        while (*s1 && (*s1 == *s2)) {
            ++s1;
            ++s2;
        }
        return static_cast<int64_t>(static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2));
    }

} // namespace COA_Reference

#endif // COA_CO3_ASSEMBLY_H
