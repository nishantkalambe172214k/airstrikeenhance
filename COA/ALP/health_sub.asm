.386
.MODEL FLAT, STDCALL
.STACK 4096

.DATA
    current_health   DWORD   40
    incoming_damage  DWORD   40
    remaining_health DWORD   ?
    game_over_flag   BYTE    0

.CODE
main PROC
    MOV     EAX, [current_health]
    MOV     EDX, [incoming_damage]
    SUB     EAX, EDX

    JZ      trigger_game_over
    JS      trigger_game_over

    MOV     [remaining_health], EAX
    MOV     [game_over_flag], 0
    JMP     exit_routine

trigger_game_over:
    MOV     [remaining_health], 0
    MOV     [game_over_flag], 1

exit_routine:
    RET
main ENDP
END main
