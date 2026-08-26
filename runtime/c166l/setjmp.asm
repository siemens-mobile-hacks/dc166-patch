$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME DC166_SETJMP
        ASSUME DPP3:SYSTEM

DC166_SETJMP_PR SECTION CODE WORD PUBLIC 'CLIBRARY'
        PUBLIC _setjmp
        PUBLIC _longjmp

_setjmp PROC FAR
        MOV R10,SP
        MOV R11,[R10+]
        EXTP R13,#01h
        MOV [R12],R11
        MOV R11,[R10]
        EXTP R13,#01h
        MOV [R12+#02h],R11
        MOV R11,SP
        EXTP R13,#01h
        MOV [R12+#04h],R11
        EXTP R13,#01h
        MOV [R12+#06h],R0
        EXTP R13,#01h
        MOV [R12+#08h],R6
        EXTP R13,#01h
        MOV [R12+#0Ah],R7
        EXTP R13,#01h
        MOV [R12+#0Ch],R8
        EXTP R13,#01h
        MOV [R12+#0Eh],R9
        MOV R4,#0
        RETS
_setjmp ENDP

_longjmp PROC FAR
        EXTP R13,#01h
        MOV R11,[R12+#04h]
        EXTP R13,#01h
        MOV R10,[R12]
        MOV [R11],R10
        ADD R11,#02h
        EXTP R13,#01h
        MOV R10,[R12+#02h]
        MOV [R11],R10
        EXTP R13,#01h
        MOV R10,[R12+#04h]
        MOV SP,R10
        EXTP R13,#01h
        MOV R0,[R12+#06h]
        EXTP R13,#01h
        MOV R6,[R12+#08h]
        EXTP R13,#01h
        MOV R7,[R12+#0Ah]
        EXTP R13,#01h
        MOV R8,[R12+#0Ch]
        EXTP R13,#01h
        MOV R9,[R12+#0Eh]
        MOV R4,R14
        JMPR cc_NE,__longjmp_return
        MOV R4,#1
__longjmp_return:
        RETS
_longjmp ENDP
DC166_SETJMP_PR ENDS

        REGDEF R0-R15
        END
