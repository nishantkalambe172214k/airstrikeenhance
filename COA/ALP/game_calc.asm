; ============================================================================
; [COA CO3] 64-BIT ASSEMBLY: COMPOUND GAME CALCULATION (x86_64 Architecture Upgrade)
; Upgraded from legacy 32-bit 80386 (EAX, EBX, ECX) to 64-bit (RAX, RBX, RCX, QWORD)
; ============================================================================

default rel

section .data
    score   dq  100                     ; [COA CO3] 64-bit Base score
    reward  dq  20                      ; [COA CO3] 64-bit Reward addition
    damage  dq  10                      ; [COA CO3] 64-bit Damage subtraction
    result  dq  0                       ; [COA CO3] 64-bit Final result

section .text
global game_calc_main
game_calc_main:
    ; [COA CO3] Load 64-bit registers RAX, RBX, RCX
    mov     rax, [score]
    mov     rbx, [reward]
    mov     rcx, [damage]

    add     rax, rbx                    ; [COA CO3] 64-bit ADD RAX, RBX
    sub     rax, rcx                    ; [COA CO3] 64-bit SUB RAX, RCX

    mov     [result], rax
    ret
