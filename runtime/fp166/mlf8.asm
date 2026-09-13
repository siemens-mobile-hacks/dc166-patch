$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_MLF8

DC166_MLF8_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __mlf8r
        PUBLIC __dmlf8
        EXTERN _dc166_df_mul:FAR

; __dmlf8 was an internal public entry shared only by the original mlf8/dvf8
; pair.  The replacement has no cross-module entry and keeps the name as an
; alias so the public symbol set remains compatible.
__dmlf8:
__mlf8r PROC FAR
        MOV [-R0],R5
        MOV [-R0],R6
        MOV [-R0],R7
        MOV [-R0],R8
        MOV [-R0],R9
        MOV [-R0],R12
        MOV [-R0],R13
        MOV [-R0],R14
        MOV [-R0],R15
        MOV [-R0],R10
        MOV [-R0],R11
        MOV R12,R10
        MOV R13,R10
        MOV R14,R11
        CALLS SEG _dc166_df_mul,_dc166_df_mul
        MOV R11,[R0+]
        MOV R10,[R0+]
        MOV R15,[R0+]
        MOV R14,[R0+]
        MOV R13,[R0+]
        MOV R12,[R0+]
        MOV R9,[R0+]
        MOV R8,[R0+]
        MOV R7,[R0+]
        MOV R6,[R0+]
        MOV R5,[R0+]
        RETS
__mlf8r ENDP
DC166_MLF8_PR ENDS

        REGDEF R0-R15
        END
