$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_ADF8

DC166_ADF8_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __adf8r
        PUBLIC __sbf8r
        PUBLIC __sbf8rr
        EXTERN _dc166_df_add:FAR
        EXTERN _dc166_df_sub:FAR

__adf8r PROC FAR
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
        CALLS SEG _dc166_df_add,_dc166_df_add
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
__adf8r ENDP

__sbf8r PROC FAR
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
        CALLS SEG _dc166_df_sub,_dc166_df_sub
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
__sbf8r ENDP

; Reverse-register form used when the compiler needs right - left.
__sbf8rr PROC FAR
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
        MOV R13,R11
        MOV R14,R10
        CALLS SEG _dc166_df_sub,_dc166_df_sub
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
__sbf8rr ENDP
DC166_ADF8_PR ENDS

        REGDEF R0-R15
        END
