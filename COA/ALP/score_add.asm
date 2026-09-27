.386
.MODEL FLAT, STDCALL
.STACK 4096

.DATA
    current_score   DWORD   1500
    score_increment DWORD   250
    final_score     DWORD   ?
    score_overflow  BYTE    0

.CODE
main PROC
    MOV     EAX, [current_score]
    MOV     EDX, [score_increment]
    ADD     EAX, EDX

    JC      handle_carry

    MOV     [final_score], EAX
    JMP     done

handle_carry:
    MOV     [score_overflow], 1
    MOV     [final_score], 0FFFFFFFFh

done:
    RET
main ENDP
END main
