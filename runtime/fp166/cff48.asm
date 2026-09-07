$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_CFF48

DC166_CFF48_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __cff48r

; Convert the IEEE-754 binary32 value in R4:R5 to binary64 on the user stack.
; The original TASKING implementation treated every exponent-zero input as
; zero, discarding all binary32 subnormal values.
__cff48r PROC FAR
        SUB R0,#08h
        MOV R10,R0

        MOV R2,R4
        AND R2,#08000h
        MOV R1,R4
        SHR R1,#07h
        AND R1,#00ffh
        AND R4,#007fh

        CMP R1,#00h
        JMPR cc_NE,__cff48_nonzero_exponent
        MOV R3,R4
        OR R3,R5
        JMPR cc_NE,__cff48_subnormal

        MOV [R10],R2
        MOV R3,#00h
        MOV [R10+#02h],R3
        MOV [R10+#04h],R3
        MOV [R10+#06h],R3
        RETS

__cff48_subnormal:
        MOV R1,#0381h
__cff48_normalize:
        JB R4.7,__cff48_pack
        SHL R5,#01h
        ADDC R4,R4
        SUB R1,#01h
        JMPR cc_UC,__cff48_normalize

__cff48_nonzero_exponent:
        CMP R1,#00ffh
        JMPR cc_EQ,__cff48_special
        ADD R1,#0380h
        JMPR cc_UC,__cff48_pack

__cff48_special:
        MOV R1,#07ffh

__cff48_pack:
        AND R4,#007fh
        MOV R3,R4
        SHR R3,#03h
        SHL R1,#04h
        OR R3,R1
        OR R3,R2
        MOV [R10],R3

        SHL R4,#0dh
        MOV R3,R5
        SHR R3,#03h
        OR R3,R4
        MOV [R10+#02h],R3

        SHL R5,#0dh
        MOV [R10+#04h],R5
        MOV R3,#00h
        MOV [R10+#06h],R3
        RETS
__cff48r ENDP
DC166_CFF48_PR ENDS

        REGDEF R0-R15
        END
