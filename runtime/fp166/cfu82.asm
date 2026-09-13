$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_CFU82

DC166_CFU82_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __cfu82r
        EXTERN _dc166_df_to_u32:FAR

__cfu82r PROC FAR
        MOV [-R0],R6
        MOV [-R0],R7
        MOV [-R0],R8
        MOV [-R0],R9
        MOV [-R0],R10
        MOV [-R0],R11
        MOV [-R0],R12
        MOV [-R0],R13
        MOV [-R0],R14
        MOV [-R0],R15
        MOV R12,R10
        CALLS SEG _dc166_df_to_u32,_dc166_df_to_u32
        MOV R15,[R0+]
        MOV R14,[R0+]
        MOV R13,[R0+]
        MOV R12,[R0+]
        MOV R11,[R0+]
        MOV R10,[R0+]
        MOV R9,[R0+]
        MOV R8,[R0+]
        MOV R7,[R0+]
        MOV R6,[R0+]
        RETS
__cfu82r ENDP
DC166_CFU82_PR ENDS

        REGDEF R0-R15
        END
