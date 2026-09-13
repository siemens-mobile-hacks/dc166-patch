$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME DC166_FP_CALLEE_SAVED

DC166_FP_CALLEE_SAVED_CODE SECTION CODE WORD PUBLIC 'CPROGRAM'

        PUBLIC _check_float_load_preserves_r6
_check_float_load_preserves_r6 PROC FAR
        MOV     [-R0],R6
        MOV     R6,#0a55ah
        MOV     R12,#00h
        MOV     R13,#01h
        CALLS   SEG _evaluate_float,_evaluate_float
        CMP     R6,#0a55ah
        JMPR    cc_Z,r6_preserved
        MOV     R4,#01h
        JMPR    cc_UC,r6_done
r6_preserved:
        MOV     R4,#00h
r6_done:
        MOV     R6,[R0+]
        RETS
_check_float_load_preserves_r6 ENDP

DC166_FP_CALLEE_SAVED_CODE ENDS

        EXTERN _evaluate_float:FAR
        REGDEF R0-R15
        END
