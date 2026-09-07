$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME DC166_ABI_DOUBLE_RETURN

ABI_DOUBLE_CODE SECTION CODE WORD PUBLIC 'CPROGRAM'

        PUBLIC  _abi_check_tasking_double_return
_abi_check_tasking_double_return PROC FAR
        MOV     R1,[R0]
        SUB     R0,#08h
        MOV     [-R0],R1
        SUB     R0,#08h
        MOV     [R0],R12
        MOV     [R0+#02h],R13
        MOV     [R0+#04h],R14
        MOV     [R0+#06h],R15
        CALLS   SEG _tasking_double_identity,_tasking_double_identity
        MOV     R1,R0
        ADD     R1,#0Ah
        CMP     R1,R4
        JMPR    cc_NZ,abi_callee_pointer
        MOV     R1,[R0]
        MOV     R2,[R4]
        CMP     R1,R2
        JMPR    cc_NZ,abi_callee_word_0
        MOV     R1,[R0+#02h]
        MOV     R2,[R4+#02h]
        CMP     R1,R2
        JMPR    cc_NZ,abi_callee_word_1
        MOV     R1,[R0+#04h]
        MOV     R2,[R4+#04h]
        CMP     R1,R2
        JMPR    cc_NZ,abi_callee_word_2
        MOV     R1,[R0+#06h]
        MOV     R2,[R4+#06h]
        CMP     R1,R2
        JMPR    cc_NZ,abi_callee_word_3
        MOV     R4,#00h
        JMPR    cc_UC,abi_callee_done
abi_callee_word_0:
        MOV     R4,#01h
        JMPR    cc_UC,abi_callee_done
abi_callee_word_1:
        MOV     R4,#02h
        JMPR    cc_UC,abi_callee_done
abi_callee_word_2:
        MOV     R4,#03h
        JMPR    cc_UC,abi_callee_done
abi_callee_word_3:
        MOV     R4,#04h
        JMPR    cc_UC,abi_callee_done
abi_callee_pointer:
        MOV     R4,#05h
abi_callee_done:
        ADD     R0,#012h
        RETS
_abi_check_tasking_double_return ENDP

        PUBLIC  _abi_r4_only_double_identity
_abi_r4_only_double_identity PROC FAR
        MOV     R4,R0
        CALLS   SEG __load8n,__load8n
        MOV     R4,#012h
        ADD     R4,R0
        MOV     R11,R4
        CALLS   SEG __store8n,__store8n
        MOV     R4,R11
        ADD     R0,#08h
        RETS
_abi_r4_only_double_identity ENDP

ABI_DOUBLE_CODE ENDS

        EXTERN  _tasking_double_identity:FAR
        EXTERN  __load8n:FAR
        EXTERN  __store8n:FAR
        REGDEF  R0-R15
        END
