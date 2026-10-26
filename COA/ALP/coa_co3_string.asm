; ============================================================================
; [COA CO3] 64-BIT ASSEMBLY STRING PROCESSING ALGORITHMS (x86_64 NASM)
; Course Outcome 3: Algorithmic and String Processing
;
; Implements syllabus-mandated string processing operations:
; 1. asm_str_length:  Computes length of null-terminated string using 64-bit pointer
; 2. asm_str_to_upper: Converts ASCII string to uppercase with buffer bounds check
; 3. asm_str_compare: Lexicographical comparison of two null-terminated strings
;
; Calling Conventions Supported:
; - System V AMD64 ABI (macOS, Linux): RDI = arg1, RSI = arg2, RDX = arg3, RAX = return
; - Microsoft x64 ABI (Windows):       RCX = arg1, RDX = arg2, R8  = arg3, RAX = return
; ============================================================================

default rel

; Cross-platform symbol naming (Mach-O 64 requires leading underscore)
%ifidn __OUTPUT_FORMAT__, macho64
    %define SYMBOL(x) _ %+ x
%else
    %define SYMBOL(x) x
%endif

; Cross-platform argument setup macros
%ifidn __OUTPUT_FORMAT__, win64
    %macro SETUP_1ARG 0
        mov rdi, rcx            ; RDI = arg1
    %endmacro
    %macro SETUP_2ARGS 0
        mov rdi, rcx            ; RDI = arg1
        mov rsi, rdx            ; RSI = arg2
    %endmacro
    %macro SETUP_3ARGS 0
        mov rdi, rcx            ; RDI = arg1 (src)
        mov rsi, rdx            ; RSI = arg2 (dst)
        mov rdx, r8             ; RDX = arg3 (max_len)
    %endmacro
%else
    %macro SETUP_1ARG 0
        ; System V AMD64 ABI: arg1 in RDI
    %endmacro
    %macro SETUP_2ARGS 0
        ; System V AMD64 ABI: arg1 in RDI, arg2 in RSI
    %endmacro
    %macro SETUP_3ARGS 0
        ; System V AMD64 ABI: arg1 in RDI, arg2 in RSI, arg3 in RDX
    %endmacro
%endif

section .text

; ----------------------------------------------------------------------------
; [COA CO3] STRING ALGORITHM 1: STRING LENGTH CALCULATION
; C++ Signature: uint64_t asm_str_length(const char* str);
;
; Concept:
; Traverses string memory starting from base pointer in RDI.
; Increments character counter RAX until null terminator byte (0x00) is reached.
; Handles NULL pointer safely by returning 0.
;
; Register Usage:
; - RDI: Pointer to string character buffer
; - RAX: Length counter accumulator (Return register)
; ----------------------------------------------------------------------------
global SYMBOL(asm_str_length)
SYMBOL(asm_str_length):
    SETUP_1ARG

    xor rax, rax                ; [COA CO3] Initialize length counter RAX = 0

    ; [COA CO3] Guard against NULL pointer
    test rdi, rdi
    jz .len_done

.len_loop:
    cmp byte [rdi + rax], 0     ; [COA CO3] Check for null terminator '\0'
    je .len_done
    inc rax                     ; [COA CO3] Increment character count
    jmp .len_loop

.len_done:
    ret


; ----------------------------------------------------------------------------
; [COA CO3] STRING ALGORITHM 2: ASCII STRING TO UPPERCASE CONVERSION
; C++ Signature: uint64_t asm_str_to_upper(const char* src, char* dst, uint64_t max_len);
;
; Concept:
; Reads characters sequentially from [src + RCX] up to max_len - 1 characters.
; If byte is in range 'a' (0x61) to 'z' (0x7A), converts to uppercase by subtracting
; 0x20 (or bitwise AND with 0xDF).
; Writes character to [dst + RCX] and appends null terminator at dst[RCX].
;
; Register Usage:
; - RDI: Source string pointer (const char* src)
; - RSI: Destination buffer pointer (char* dst)
; - RDX: Maximum destination buffer capacity (max_len)
; - RCX: Index / processed character counter
; - RAX: Count of characters processed and written (Return register)
; - R8B: Scratch 8-bit register for character transformation
; ----------------------------------------------------------------------------
global SYMBOL(asm_str_to_upper)
SYMBOL(asm_str_to_upper):
    SETUP_3ARGS

    xor rax, rax                ; [COA CO3] Default return count = 0

    ; [COA CO3] Null checks and capacity check
    test rdi, rdi
    jz .upper_exit
    test rsi, rsi
    jz .upper_exit
    test rdx, rdx
    jz .upper_exit

    ; If max_len == 1, we can only place the null terminator
    cmp rdx, 1
    jle .write_null_only

    xor rcx, rcx                ; [COA CO3] RCX = loop index counter (0)
    dec rdx                     ; [COA CO3] Reserve 1 byte for null terminator (limit = max_len - 1)

.upper_loop:
    cmp rcx, rdx                ; Check if buffer limit reached
    jge .terminate_dst

    mov r8b, byte [rdi + rcx]   ; [COA CO3] Load character from src
    test r8b, r8b               ; Check for null terminator '\0'
    jz .terminate_dst

    ; [COA CO3] ASCII check: if ('a' <= char <= 'z') char -= 0x20
    cmp r8b, 'a'
    jl .store_char
    cmp r8b, 'z'
    jg .store_char
    sub r8b, 0x20               ; [COA CO3] Convert lowercase ASCII to uppercase

.store_char:
    mov byte [rsi + rcx], r8b   ; [COA CO3] Store converted character into destination
    inc rcx                     ; [COA CO3] Advance loop index
    jmp .upper_loop

.terminate_dst:
    mov byte [rsi + rcx], 0     ; [COA CO3] Ensure null terminator at destination
    mov rax, rcx                ; [COA CO3] Return number of characters written
    ret

.write_null_only:
    mov byte [rsi], 0           ; Zero-terminate empty destination buffer
    xor rax, rax
.upper_exit:
    ret


; ----------------------------------------------------------------------------
; [COA CO3] STRING ALGORITHM 3: LEXICOGRAPHICAL STRING COMPARISON
; C++ Signature: int64_t asm_str_compare(const char* s1, const char* s2);
;
; Concept:
; Compares strings s1 and s2 byte-by-byte using unsigned values.
; Returns:
;   0  if strings are identical
;  < 0 if s1 < s2
;  > 0 if s1 > s2
;
; Register Usage:
; - RDI: Pointer to string 1
; - RSI: Pointer to string 2
; - RCX: Loop index
; - R8B, R9B: Character bytes from s1 and s2
; - RAX: Signed difference result (Return register)
; ----------------------------------------------------------------------------
global SYMBOL(asm_str_compare)
SYMBOL(asm_str_compare):
    SETUP_2ARGS

    ; [COA CO3] Handle NULL pointer corner cases
    cmp rdi, rsi
    je .cmp_equal               ; Same pointer (or both NULL) -> equal
    test rdi, rdi
    jz .s1_null                 ; s1 is NULL, s2 is not -> s1 < s2
    test rsi, rsi
    jz .s2_null                 ; s2 is NULL, s1 is not -> s1 > s2

    xor rcx, rcx                ; [COA CO3] RCX = index (0)

.cmp_loop:
    movzx r8d, byte [rdi + rcx] ; [COA CO3] Read byte from s1
    movzx r9d, byte [rsi + rcx] ; [COA CO3] Read byte from s2

    cmp r8d, r9d
    jne .cmp_diff               ; Mismatch found -> compute difference

    test r8d, r8d               ; If byte is null, strings are equal and finished
    jz .cmp_equal

    inc rcx                     ; Advance to next character
    jmp .cmp_loop

.cmp_diff:
    sub r8d, r9d                ; [COA CO3] Difference = byte(s1) - byte(s2)
    movsxd rax, r8d             ; Sign-extend 32-bit difference into 64-bit RAX
    ret

.cmp_equal:
    xor rax, rax                ; Return 0 (Equal)
    ret

.s1_null:
    mov rax, -1                 ; NULL s1 < non-null s2
    ret

.s2_null:
    mov rax, 1                  ; Non-null s1 > NULL s2
    ret
