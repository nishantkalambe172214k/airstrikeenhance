; ============================================================================
; [COA CO3] 64-BIT ASSEMBLY: SCORE ADDITION (x86_64 Architecture Upgrade)
; Upgraded from legacy 32-bit 80386 (EAX, EDX, DWORD) to 64-bit (RAX, RDX, QWORD)
; ============================================================================

default rel

section .data
    current_score   dq  1500            ; [COA CO3] 64-bit Quadword integer
    score_increment dq  250             ; [COA CO3] 64-bit Quadword integer
    final_score     dq  0               ; [COA CO3] 64-bit result holder
    score_overflow  db  0               ; Overflow indicator flag

section .text
global score_add_main
score_add_main:
    ; [COA CO3] Load 64-bit registers RAX and RDX
    mov     rax, [current_score]
    mov     rdx, [score_increment]
    add     rax, rdx                    ; [COA CO3] 64-bit ADD RAX, RDX

    jc      handle_carry                ; Jump if carry flag (CF=1) set

    mov     [final_score], rax
    jmp     done

handle_carry:
    mov     byte [score_overflow], 1
    mov     qword [final_score], 0xFFFFFFFFFFFFFFFF ; 64-bit maximum clamp

done:
    ret
