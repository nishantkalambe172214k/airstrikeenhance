; ============================================================================
; [COA CO3] 64-BIT ASSEMBLY: HEALTH SUBTRACTION (x86_64 Architecture Upgrade)
; Upgraded from legacy 32-bit 80386 (EAX, EDX, DWORD) to 64-bit (RAX, RDX, QWORD)
; ============================================================================

default rel

section .data
    current_health   dq  40             ; [COA CO3] 64-bit Quadword integer
    incoming_damage  dq  40             ; [COA CO3] 64-bit Quadword integer
    remaining_health dq  0              ; [COA CO3] 64-bit result holder
    game_over_flag   db  0              ; Flag indicating zero/negative health

section .text
global health_sub_main
health_sub_main:
    ; [COA CO3] Load 64-bit registers RAX and RDX
    mov     rax, [current_health]
    mov     rdx, [incoming_damage]
    sub     rax, rdx                    ; [COA CO3] 64-bit SUB RAX, RDX

    jz      trigger_game_over           ; Zero Flag (ZF=1): health dropped exactly to zero
    js      trigger_game_over           ; Sign Flag (SF=1): health became negative

    mov     [remaining_health], rax
    mov     byte [game_over_flag], 0
    jmp     exit_routine

trigger_game_over:
    mov     qword [remaining_health], 0
    mov     byte [game_over_flag], 1

exit_routine:
    ret
