.386
.MODEL FLAT, STDCALL
.STACK 4096

.DATA
    score   DWORD   100
    reward  DWORD   20
    damage  DWORD   10
    result  DWORD   ?

.CODE
main PROC
    MOV     EAX, [score]
    MOV     EBX, [reward]
    MOV     ECX, [damage]

    ADD     EAX, EBX
    SUB     EAX, ECX

    MOV     [result], EAX
    RET
main ENDP
END main
