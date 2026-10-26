# Computer Organization & Architecture (COA) — CO-3 Viva & Implementation Guide

## Course Outcome 3: Algorithmic and String Processing

---

### 1. Executive Summary & Architecture Upgrade

In accordance with academic requirements for **COA CO-3 (Algorithmic and String Processing)**, the AirStriker COA module has undergone an architectural upgrade from legacy **32-bit 80386** to **64-bit x86_64**, coupled with native Assembly implementations written in **NASM** (Netwide Assembler).

| Architectural Metric | Legacy 32-Bit (CO-1 / CO-2) | Modern 64-Bit Architecture (CO-3) |
| :--- | :--- | :--- |
| **Primary Registers** | `EAX`, `EDX` (32-bit) | `RAX`, `RDX`, `RCX`, `RBX` (64-bit Quadwords) |
| **Data Word Size** | `DWORD` (4 bytes, 32 bits) | `QWORD` (8 bytes, 64 bits) |
| **Address Space** | Flat 32-bit ($2^{32} \approx 4\text{ GB}$) | Flat 64-bit ($2^{64} \approx 16\text{ Exabytes}$) |
| **Status Register** | `EFLAGS` (`ZF`, `CF`, `SF`, `OF`) | `RFLAGS` (`ZF`, `CF`, `SF`, `OF` on 64-bit arithmetic) |
| **ALU Core Class** | [`ALU80386`](file:///Users/nishant/airstrikeenhance/COA/ALU/ALU80386.h) | [`ALU_x86_64`](file:///Users/nishant/airstrikeenhance/COA/ALU/ALU80386.h) (backward-compatible alias `ALU80386`) |
| **Subregister Semantics**| N/A | Writing 32-bit `EAX`/`EDX` automatically zero-extends upper 32-bits of `RAX`/`RDX` |
| **Calling Conventions** | cdecl / stdcall (stack-based) | System V AMD64 (Unix/macOS) & Microsoft x64 ABI (Windows) |
| **Arithmetic Algorithms**| Software ADD/SUB | 64-bit Add-and-Shift Multiplier & Successive Addition in NASM |
| **String Algorithms** | Standard C-library | Native 64-bit Assembly String Length, Uppercase, & Compare |

---

### 2. File Organization & Implementations

1. [**`COA/ALP/coa_co3_arithmetic.asm`**](file:///Users/nishant/airstrikeenhance/COA/ALP/coa_co3_arithmetic.asm)
   - Implements 64-bit **Add-and-Shift Binary Multiplier** (`asm_multiply_shift_add`).
   - Implements 64-bit **Successive Addition Multiplier** (`asm_multiply_successive`).
   - Cross-platform ABI macros for System V AMD64 (`macho64`/`elf64`) and Microsoft x64 (`win64`).

2. [**`COA/ALP/coa_co3_string.asm`**](file:///Users/nishant/airstrikeenhance/COA/ALP/coa_co3_string.asm)
   - Implements 64-bit **String Length** (`asm_str_length`) via pointer traversal.
   - Implements 64-bit **ASCII Uppercase Conversion** (`asm_str_to_upper`) with buffer capacity boundary checks.
   - Implements 64-bit **Lexicographical String Comparison** (`asm_str_compare`).

3. [**`COA/ALP/COA_CO3_Assembly.h`**](file:///Users/nishant/airstrikeenhance/COA/ALP/COA_CO3_Assembly.h)
   - C/C++ header defining `extern "C"` assembly prototypes and `COA_Reference` validation functions.

4. [**`COA/ALU/ALU80386.h`**](file:///Users/nishant/airstrikeenhance/COA/ALU/ALU80386.h) & [**`ALU80386.cpp`**](file:///Users/nishant/airstrikeenhance/COA/ALU/ALU80386.cpp)
   - 64-bit ALU class `ALU_x86_64` managing 64-bit registers (`RAX`, `RDX`, `RCX`, `RBX`) and RFLAGS.
   - Preserves 32-bit backward compatibility accessors (`setEAX`, `getEAX`, `executeADD`, `executeSUB`).
   - Integrates hardware multiplication algorithms and string operations.

5. [**`COA/ALU/test_coa_co3.cpp`**](file:///Users/nishant/airstrikeenhance/COA/ALU/test_coa_co3.cpp)
   - Comprehensive unit test suite covering 32-bit compatibility, 64-bit ALU flags, multiplication algorithms, and string processing.

6. [**`run_coa_tests.sh`**](file:///Users/nishant/airstrikeenhance/run_coa_tests.sh) & [**`run_coa_tests.bat`**](file:///Users/nishant/airstrikeenhance/run_coa_tests.bat)
   - Cross-platform build and execution scripts.

7. [**`CGL/src/MainGame.cpp`**](file:///Users/nishant/airstrikeenhance/CGL/src/MainGame.cpp)
   - Gameplay integration: Wave combo bounty calculated using 64-bit Add-and-Shift binary multiplication in NASM.
   - Callsign normalization and display in HUD telemetry banner using 64-bit string processing.

---

### 3. Detailed Algorithm Formulations

#### 3.1 Arithmetic Algorithm 1: Add-and-Shift Binary Hardware Multiplication
*Function:* `asm_multiply_shift_add(uint64_t multiplicand, uint64_t multiplier)`
- **Mathematical Principle:**
  In digital binary hardware, multiplication $A \times B$ is computed by inspecting the least significant bit (LSB) of multiplier $Q$:
  $$\text{Product} = \sum_{i=0}^{n-1} Q_i \cdot (M \ll i)$$
- **Step-by-step Assembly Logic:**
  1. Initialize Accumulator $\text{RAX} = 0$.
  2. If either operand is 0 (`test rdi, rdi` / `test rsi, rsi`), return 0.
  3. Inspect LSB of multiplier $Q$ (`test rsi, 1`).
  4. If LSB == 1, add multiplicand $M$ into accumulator (`add rax, rdi`).
  5. Shift multiplier right by 1 bit (`shr rsi, 1`).
  6. Shift multiplicand left by 1 bit (`shl rdi, 1`).
  7. Loop until multiplier becomes 0 (`jz .done`).
- **Computational Complexity:** $O(\log_2(Q))$ additions, compared to $O(Q)$ in naive addition.

#### 3.2 Arithmetic Algorithm 2: Successive Addition
*Function:* `asm_multiply_successive(uint64_t multiplicand, uint64_t multiplier)`
- **Mathematical Principle:**
  Repeated addition of the multiplicand:
  $$\text{Product} = \underbrace{M + M + \dots + M}_{Q \text{ times}}$$
- **Step-by-step Assembly Logic:**
  1. Initialize Accumulator $\text{RAX} = 0$.
  2. Initialize loop counter $\text{RCX} = Q$.
  3. Execute `add rax, rdi` followed by `dec rcx`.
  4. Jump if not zero (`jnz .successive_loop`).

#### 3.3 String Processing Algorithms
- **String Length (`asm_str_length`):**
  Uses 64-bit base pointer in `RDI`, testing `[rdi + rax]` against `0` in a tight loop. Handles `NULL` pointers safely by returning `0`.
- **String Uppercase (`asm_str_to_upper`):**
  Reads characters up to `max_len - 1`. If `char >= 'a' && char <= 'z'`, executes `sub r8b, 0x20` and writes to destination buffer. Appends null terminator `0x00` at `dst[rcx]`.
- **String Compare (`asm_str_compare`):**
  Loads unsigned bytes into 32-bit registers `r8d` and `r9d`, comparing until mismatch or null terminator. Computes signed difference `r8d - r9d` and sign-extends into `RAX` via `movsxd`.

---

### 4. Cross-Platform Calling Convention Mapping

| Argument | System V AMD64 ABI (macOS, Linux) | Microsoft x64 ABI (Windows) | Macro Setup |
| :--- | :--- | :--- | :--- |
| **Arg 1 (Multiplicand / Src / Str1)** | `RDI` | `RCX` | `mov rdi, rcx` |
| **Arg 2 (Multiplier / Dst / Str2)** | `RSI` | `RDX` | `mov rsi, rdx` |
| **Arg 3 (Buffer Max Length)** | `RDX` | `R8` | `mov rdx, r8` |
| **Return Value** | `RAX` | `RAX` | Identical (`RAX`) |
| **Symbol Naming** | Leading underscore `_func` (Mach-O) | No prefix `func` (PE-COFF) | `%define SYMBOL(x) _ %+ x` |

---

### 5. Viva Voce Questions & Answers

**Q1: Why upgrade from 32-bit to 64-bit architecture?**
> **Answer:** Modern computer architectures use x86_64 with 64-bit wide registers (`RAX`, `RDX`, `RCX`, `RBX`), enabling single-cycle arithmetic on numbers up to $2^{64}-1$ ($1.84 \times 10^{19}$), larger address space, flat memory models, and 16 general-purpose registers (`RAX` through `R15`) compared to only 8 in 32-bit x86.

**Q2: What happens when you write to a 32-bit register like EAX in x86_64?**
> **Answer:** In x86_64, writing to any 32-bit subregister (e.g., `MOV EAX, EBX`) automatically zero-extends the value into the entire 64-bit register (`RAX`), clearing the upper 32 bits. This design avoids partial-register stall penalties in modern superscalar processors. Our `ALU_x86_64::setEAX()` replicates this exact hardware behavior.

**Q3: How does the Add-and-Shift multiplication algorithm work in hardware?**
> **Answer:** It emulates binary pencil-and-paper multiplication. For every bit in the multiplier, if the bit is 1, the shifted multiplicand is added to the accumulator. The multiplier is right-shifted by 1 bit, and the multiplicand is left-shifted by 1 bit. It completes in at most 64 cycles for a 64-bit word, far faster than successive addition which requires up to $2^{64}-1$ iterations.

**Q4: How do calling conventions differ between Windows and macOS/Linux on 64-bit?**
> **Answer:** Linux and macOS use the System V AMD64 ABI, passing the first six integer/pointer arguments in `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9`. Windows uses the Microsoft x64 ABI, passing arguments in `RCX`, `RDX`, `R8`, `R9`. Both return integer values in `RAX`. Our assembly routines use NASM macros to dynamically map arguments according to `__OUTPUT_FORMAT__`.

**Q5: How is COA CO-3 integrated into the AirStriker game?**
> **Answer:** 
> 1. Arithmetic: In `CGL/src/MainGame.cpp`, when an enemy ship is destroyed, the score bounty is multiplied by the wave combo multiplier using the native 64-bit assembly Add-and-Shift multiplier (`alu.computeScoreMultiplier`).
> 2. String Processing: The pilot's callsign (`"viper-strike_01"`) is normalized to uppercase (`"VIPER-STRIKE_01"`) and validated for length using `asm_str_to_upper` and `asm_str_length`, displaying the result in real time on the HUD.
