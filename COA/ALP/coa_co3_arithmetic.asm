; ============================================================================
; [COA CO3] 64-BIT ASSEMBLY ARITHMETIC ALGORITHMS (x86_64 NASM)
; Course Outcome 3: Algorithmic and String Processing
;
; Implements two syllabus-mandated multiplication algorithms:
; 1. asm_multiply_shift_add: Add-and-Shift (Shift-and-Add) binary multiplier
; 2. asm_multiply_successive: Successive addition loop multiplier
;
; Calling Conventions Supported:
; - System V AMD64 ABI (macOS, Linux): RDI = arg1, RSI = arg2, RAX = return
; - Microsoft x64 ABI (Windows):       RCX = arg1, RDX = arg2, RAX = return
; ============================================================================

default rel

; Cross-platform symbol naming (Mach-O 64 requires leading underscore)
%ifidn __OUTPUT_FORMAT__, macho64
    %define SYMBOL(x) _ %+ x
%else
    %define SYMBOL(x) x
%endif

; Cross-platform argument setup macro for 2-argument functions
%ifidn __OUTPUT_FORMAT__, win64
    %macro SETUP_2ARGS 0
        mov rdi, rcx            ; RDI = Multiplicand (arg1)
        mov rsi, rdx            ; RSI = Multiplier   (arg2)
    %endmacro
%else
    %macro SETUP_2ARGS 0
        ; Arguments already in RDI (arg1) and RSI (arg2) under System V AMD64 ABI
    %endmacro
%endif

section .text

; ----------------------------------------------------------------------------
; [COA CO3] ALGORITHM 1: ADD-AND-SHIFT (SHIFT-AND-ADD) MULTIPLICATION
; C++ Signature: uint64_t asm_multiply_shift_add(uint64_t multiplicand, uint64_t multiplier);
;
; Concept:
; Simulates hardware ALU binary multiplication:
; For each bit in multiplier Q:
;   If Q_0 == 1: Accumulator (A) = A + Multiplicand (M)
;   Shift Multiplicand left  (M = M << 1)
;   Shift Multiplier right   (Q = Q >> 1)
; Loops until Multiplier Q reaches 0. Time Complexity: O(log2(Q)) additions.
;
; Register Usage:
; - RDI: Multiplicand (M), progressively shifted left
; - RSI: Multiplier (Q), progressively shifted right
; - RAX: Product accumulator (Return register)
; ----------------------------------------------------------------------------
global SYMBOL(asm_multiply_shift_add)
SYMBOL(asm_multiply_shift_add):
    SETUP_2ARGS

    xor rax, rax                ; [COA CO3] Clear accumulator (Product = 0)

    ; [COA CO3] Edge case checks: if either operand is 0, product is immediately 0
    test rdi, rdi
    jz .done
    test rsi, rsi
    jz .done

.shift_add_loop:
    ; [COA CO3] Step 1: Test LSB of multiplier (Q_0)
    test rsi, 1
    jz .skip_add                ; If LSB == 0, skip addition step

    ; [COA CO3] Step 2: Add multiplicand to accumulator (A = A + M)
    add rax, rdi

.skip_add:
    ; [COA CO3] Step 3: Shift multiplier right by 1 bit (Q = Q >> 1)
    shr rsi, 1
    jz .done                    ; Early exit: if no more set bits remain, terminate

    ; [COA CO3] Step 4: Shift multiplicand left by 1 bit (M = M << 1)
    shl rdi, 1
    jmp .shift_add_loop

.done:
    ret


; ----------------------------------------------------------------------------
; [COA CO3] ALGORITHM 2: SUCCESSIVE ADDITION MULTIPLICATION
; C++ Signature: uint64_t asm_multiply_successive(uint64_t multiplicand, uint64_t multiplier);
;
; Concept:
; Computes multiplication by repeated addition:
;   Product = M + M + M + ... (Q times)
;
; Register Usage:
; - RDI: Multiplicand (M)
; - RSI / RCX: Multiplier loop counter (decrements to 0)
; - RAX: Product accumulator (Return register)
; ----------------------------------------------------------------------------
global SYMBOL(asm_multiply_successive)
SYMBOL(asm_multiply_successive):
    SETUP_2ARGS

    xor rax, rax                ; [COA CO3] Clear accumulator (Product = 0)

    ; [COA CO3] Edge case checks: if either operand is 0, return 0
    test rdi, rdi
    jz .done
    test rsi, rsi
    jz .done

    mov rcx, rsi                ; [COA CO3] Initialize loop counter RCX = Multiplier

.successive_loop:
    add rax, rdi                ; [COA CO3] A = A + M
    dec rcx                     ; [COA CO3] Decrement iteration counter
    jnz .successive_loop        ; Loop until counter is zero

.done:
    ret
