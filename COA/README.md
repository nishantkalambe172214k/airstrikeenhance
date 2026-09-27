# Computer Organization & Architecture (COA) — Syllabus Verification & Implementation

**Course Mapping:** CO-1 (ALU Design, Arithmetic Micro-operations, General Purpose Registers, Status Flags)  
**Academic Requirement:** COA Lab Practical No. 1 — *Design your ALU using CPU simulator for arithmetic operations*  

---

## 1. Required CPU Simulator Identification

From the curriculum syllabus and college lab assignments:
- **Designated Course Simulator:** **YASMIN CPU-OS Simulator** (Version: 7.5.50, by Besim Mustafa, Edge Hill University, UK).
- **Architecture Model:**
  - **Word Size:** 32-bit registers and address space.
  - **General Purpose Registers:** 32 registers (`R00` through `R31`).
  - **Special CPU Registers:** `PC` (Program Counter), `SP` (Stack Pointer, default 8096), `SR` (Status Register), `BR` (Base Register), `IR` (Instruction Register), `MAR`, `MDR`.
  - **Status Register (SR) Flags:**
    - `Z` (Zero Flag): Set when an arithmetic result is 0.
    - `N` (Negative Flag): Set when an arithmetic result is negative.
    - `OV` (Overflow Flag): Set when signed arithmetic overflows.

---

## 2. Clear Architectural Separation

To ensure academic integrity and grading rigor, this module maintains three distinct layers:

```
+-----------------------------------------------------------------------------------+
| 1. COA PRACTICAL IMPLEMENTATION (Academic Lab Assignment)                         |
|    - Software: YASMIN CPU-OS Simulator (Version 7.5.50)                           |
|    - Assembly Programs: yasmin_score_add.asm, yasmin_health_sub.asm               |
|    - Architecture: R00, R01 registers, Status Register Flags (Z, OV, N)          |
|    - Verification: Direct execution inside simulator RAM and register set         |
+-----------------------------------------------------------------------------------+
                                          |
                                (Conceptual Equivalence)
                                          v
+-----------------------------------------------------------------------------------+
| 2. CONCEPTUAL / ARCHITECTURAL MAPPING                                             |
|    - Score Increment: MOV #bounty, R00 -> MOV #score, R01 -> ADD R00, R01         |
|    - Hull Damage:     MOV #damage, R00 -> MOV #health, R01 -> SUB R00, R01        |
|    - Lethal Breach:   Health - Damage = 0 -> Status Flag Z=1 triggers GAME OVER   |
+-----------------------------------------------------------------------------------+
                                          |
                                (Software Emulation)
                                          v
+-----------------------------------------------------------------------------------+
| 3. AIRSTRIKER GAME INTEGRATION (Real-Time Game Engine)                            |
|    - C++ Module: ALU80386.cpp / ALU80386.h                                        |
|    - Caller: MainGame.cpp in Computer Graphics Lab (CGL)                          |
|    - Purpose: Real-time C++ frame cycle emulation so the playable game engine    |
|      computes score additions and health reductions without runtime lag.          |
+-----------------------------------------------------------------------------------+
```

---

## 3. Analysis: Can x86 `.asm` Run on YASMIN CPU-OS Simulator?

> [!WARNING]
> **NO.** The previously authored files `score_add.asm` and `health_sub.asm` use **Intel x86 / MASM Protected Mode** syntax (`.386`, `.MODEL FLAT`, `MOV EAX, [current_score]`, `ADD EAX, EDX`, `JC handle_carry`).
> 
> The **YASMIN CPU-OS Simulator** does not implement x86 machine instructions. Instead, it implements a proprietary educational RISC-like instruction set with registers `R00` to `R31`, immediate prefix `#`, and two-operand syntax `OP Source, Destination`.

Consequently, we provide **both**:
1. **YASMIN CPU-OS Simulator Native Assembly** (for lab evaluation in YASMIN):
   - [`COA/ALP/yasmin_score_add.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_score_add.asm)
   - [`COA/ALP/yasmin_health_sub.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_health_sub.asm)
   - [`COA/ALP/yasmin_health_lethal.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/yasmin_health_lethal.asm)
2. **Intel 80386 Assembly** (for x86 ALP reference):
   - [`COA/ALP/score_add.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/score_add.asm)
   - [`COA/ALP/health_sub.asm`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/COA/ALP/health_sub.asm)

---

## 4. YASMIN CPU-OS Simulator Programs & Execution Traces

### Program 1: Score Addition (`yasmin_score_add.asm`)
```assembly
; AirStriker — Score Addition in YASMIN CPU-OS Simulator
MOV #250, R00    ; R00 = Bounty points awarded for destroying alien drone
MOV #1500, R01   ; R01 = Current player score
ADD R00, R01     ; R01 = R01 + R00 = 1750 (Updated score)
HLT              ; Halts simulator
```
- **Simulator Execution Trace**:
  - `PAdd 0000`: `MOV #250, R00` -> `R00` loads `250`
  - `PAdd 0006`: `MOV #1500, R01` -> `R01` loads `1500`
  - `PAdd 0012`: `ADD R00, R01` -> ALU adds `R00` to `R01`, `R01 = 1750`
  - `PAdd 0017`: `HLT` -> Simulator pauses execution
  - **Register State**: `R00 = 250`, `R01 = 1750`, `SR = 0` (`Z=0`, `OV=0`, `N=0`).

### Program 2: Hull Damage Subtraction (`yasmin_health_sub.asm`)
```assembly
; AirStriker — Health Subtraction (Surviving Hit)
MOV #25, R00     ; R00 = Incoming enemy collision damage
MOV #100, R01    ; R01 = Current ship hull points
SUB R00, R01     ; R01 = R01 - R00 = 75
HLT
```
- **Simulator Execution Trace**:
  - `PAdd 0000`: `MOV #25, R00` -> `R00 = 25`
  - `PAdd 0006`: `MOV #100, R01` -> `R01 = 100`
  - `PAdd 0012`: `SUB R00, R01` -> ALU computes `100 - 25 = 75`, stores in `R01`
  - `PAdd 0017`: `HLT`
  - **Register State**: `R00 = 25`, `R01 = 75`, `SR = 0` (`Z=0`: Ship survives).

### Program 3: Lethal Damage & Zero Flag (`yasmin_health_lethal.asm`)
```assembly
; AirStriker — Lethal Collision & Zero Flag Trigger
MOV #40, R00     ; R00 = Lethal ramming damage
MOV #40, R01     ; R01 = Remaining hull points
SUB R00, R01     ; R01 = 40 - 40 = 0 (Hull destroyed)
HLT
```
- **Simulator Execution Trace**:
  - ALU executes subtraction: `40 - 40 = 0`.
  - **Status Register (SR)**: **`Z` (Zero Flag) is set to 1**.
  - In the flight computer, `Z=1` triggers the Game Over state.

---

## 5. Actual Simulator Output & Screenshots

Below are the verified outputs and screenshots captured directly from the **YASMIN CPU-OS Simulator** for Practical 1:

### 1. Instruction Entry Dialog in RAM
The YASMIN simulator allows instructions to be added via the Instruction Entry dialog:
- `Data Transfer -> MOV`: Loads immediate literal value into destination register (`R00`, `R01`).
- `Arithmetic -> ADD / SUB / MUL / DIV`: Selects source and destination registers.
- `Control Transfer -> HLT`: Halts execution.

### 2. Verified Addition Output in YASMIN
- Instruction sequence:
  ```
  0000 MOV #20, R00
  0006 MOV #40, R01
  0012 ADD R00, R01
  0017 HLT
  ```
- **Resulting Registers in YASMIN**:
  - `R00`: `20`
  - `R01`: `60` (`20 + 40 = 60`)
  - `PC`: `17`
  - `SR`: `0`
  - Screenshot: [`docs/screenshots/coa/yasmin_alu_addition_result.png`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/screenshots/coa/yasmin_alu_addition_result.png)

### 3. Verified Subtraction Output in YASMIN
- Instruction sequence:
  ```
  0000 MOV #20, R00
  0006 MOV #40, R01
  0012 SUB R00, R01
  0017 HLT
  ```
- **Resulting Registers in YASMIN**:
  - `R00`: `20`
  - `R01`: `20` (`40 - 20 = 20`)
  - `PC`: `17`
  - `SR`: `0`
  - Screenshot: [`docs/screenshots/coa/yasmin_alu_subtraction_result.png`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/screenshots/coa/yasmin_alu_subtraction_result.png)

### 4. Verified Multiplication Output in YASMIN
- Instruction sequence:
  ```
  0000 MOV #20, R00
  0006 MOV #40, R01
  0012 MUL R00, R01
  0017 HLT
  ```
- **Resulting Registers in YASMIN**:
  - `R00`: `20`
  - `R01`: `800` (`20 * 40 = 800`)
  - Screenshot: [`docs/screenshots/coa/yasmin_alu_multiplication_result.png`](file:///c:/Users/lenovo/OneDrive/Desktop/Refoirm/SkyStriker/docs/screenshots/coa/yasmin_alu_multiplication_result.png)

---

## 6. Standalone Virtual Machine (`cpu_vm.exe`)

We have also built the official portable Virtual Machine engine provided by Besim Mustafa for the CPU-OS Simulator:
- **Location**: `COA/vm/cpu_vm.exe`
- **Source Files**:
  - `COA/vm/Virtual Machine.cpp`
  - `COA/vm/Instruction Library.cpp`
  - `COA/vm/Support Library.cpp`
- **Compilation**:
  ```bat
  g++ -std=c++11 "COA/vm/Instruction Library.cpp" "COA/vm/Support Library.cpp" "COA/vm/Virtual Machine.cpp" -o "COA/vm/cpu_vm.exe"
  ```
- **Usage**:
  ```bat
  .\COA\vm\cpu_vm.exe r <compiled_bytecode.bin>
  ```
