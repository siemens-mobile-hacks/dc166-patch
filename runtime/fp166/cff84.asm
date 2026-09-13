$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_CFF84

DC166_CFF84_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __cff84r

; Convert the IEEE-754 binary64 value at the top of the user stack to
; binary32 in R4:R5.  The input is MSW first and remains on the stack.
__cff84r PROC FAR
        MOV [-R0],R9
        MOV [-R0],R8
        MOV [-R0],R7
        MOV [-R0],R6

        MOV R6,[R0+#08h]
        MOV R7,[R0+#0ah]
        MOV R8,[R0+#0ch]
        MOV R9,[R0+#0eh]

        MOV R4,R6
        AND R4,#08000h
        MOV R1,R6
        SHR R1,#04h
        AND R1,#07ffh

        CMP R1,#00h
        JMPA cc_EQ,__cff84_zero
        CMP R1,#07ffh
        JMPA cc_EQ,__cff84_special
        CMP R1,#0368h
        JMPA cc_ULE,__cff84_zero
        CMP R1,#0380h
        JMPA cc_ULE,__cff84_subnormal
        CMP R1,#047eh
        JMPA cc_UGT,__cff84_infinity

; A normal result retains bits 52..29.  Bits 28..0 select the
; round-to-nearest, ties-to-even increment.
__cff84_normal:
        SUB R1,#0380h
        SHL R1,#07h
        OR R4,R1

        MOV R2,R6
        AND R2,#0fh
        SHL R2,#03h
        OR R4,R2
        MOV R2,R7
        SHR R2,#0dh
        OR R4,R2

        MOV R5,R7
        SHL R5,#03h
        MOV R2,R8
        SHR R2,#0dh
        OR R5,R2

        MOV R2,R8
        AND R2,#01000h
        CMP R2,#00h
        JMPR cc_EQ,__cff84_done
        MOV R2,R8
        AND R2,#0fffh
        OR R2,R9
        JMPR cc_NE,__cff84_round
        MOV R2,R5
        AND R2,#01h
        CMP R2,#00h
        JMPR cc_EQ,__cff84_done

__cff84_round:
        ADD R5,#01h
        ADDC R4,#00h
        JMPR cc_UC,__cff84_done

; For binary32 subnormals, round the 53-bit significand after shifting it
; by 926 minus the binary64 biased exponent (30..53 places).
__cff84_subnormal:
        AND R6,#0fh
        OR R6,#010h
        MOV R3,#039eh
        SUB R3,R1
        MOV R1,R3
        MOV R2,#00h

__cff84_shift_loop:
        CMP R1,#01h
        JMPR cc_EQ,__cff84_guard_shift
        MOV R3,R9
        AND R3,#01h
        OR R2,R3

        MOV R3,R8
        SHL R3,#0fh
        SHR R9,#01h
        OR R9,R3
        MOV R3,R7
        SHL R3,#0fh
        SHR R8,#01h
        OR R8,R3
        MOV R3,R6
        SHL R3,#0fh
        SHR R7,#01h
        OR R7,R3
        SHR R6,#01h
        SUB R1,#01h
        JMPR cc_UC,__cff84_shift_loop

__cff84_guard_shift:
        MOV R1,R9
        AND R1,#01h
        MOV R3,R8
        SHL R3,#0fh
        SHR R9,#01h
        OR R9,R3
        MOV R3,R7
        SHL R3,#0fh
        SHR R8,#01h
        OR R8,R3
        MOV R3,R6
        SHL R3,#0fh
        SHR R7,#01h
        OR R7,R3
        SHR R6,#01h

        MOV R5,R9
        OR R4,R8
        CMP R1,#00h
        JMPR cc_EQ,__cff84_done
        MOV R3,R5
        AND R3,#01h
        OR R3,R2
        CMP R3,#00h
        JMPR cc_EQ,__cff84_done
        ADD R5,#01h
        ADDC R4,#00h
        JMPR cc_UC,__cff84_done

__cff84_special:
        OR R4,#07f80h
        MOV R2,R6
        AND R2,#0fh
        MOV R3,R2
        OR R3,R7
        OR R3,R8
        OR R3,R9
        CMP R3,#00h
        JMPR cc_EQ,__cff84_special_low

        SHL R2,#03h
        OR R4,R2
        MOV R2,R7
        SHR R2,#0dh
        OR R4,R2
        OR R4,#040h
        MOV R5,R7
        SHL R5,#03h
        MOV R2,R8
        SHR R2,#0dh
        OR R5,R2
        JMPR cc_UC,__cff84_done

__cff84_special_low:
        MOV R5,#00h
        JMPR cc_UC,__cff84_done

__cff84_infinity:
        OR R4,#07f80h
__cff84_zero:
        MOV R5,#00h

__cff84_done:
        MOV R6,[R0+]
        MOV R7,[R0+]
        MOV R8,[R0+]
        MOV R9,[R0+]
        RETS
__cff84r ENDP
DC166_CFF84_PR ENDS

        REGDEF R0-R15
        END
