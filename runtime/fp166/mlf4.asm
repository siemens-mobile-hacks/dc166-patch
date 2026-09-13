$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_MLF4

DC166_MLF4_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __mlf4r
        EXTERN _dc166_sf_mul:FAR

__mlf4r PROC FAR
        MOV [-R0],R6
        MOV [-R0],R7
        MOV [-R0],R8
        MOV [-R0],R9
        MOV [-R0],R12
        MOV [-R0],R13
        MOV [-R0],R14
        MOV [-R0],R15
        MOV R12,R5
        MOV R13,R4
        MOV R14,R11
        MOV R15,R10
        CALLS SEG _dc166_sf_mul,_dc166_sf_mul
        XOR R4,R5
        XOR R5,R4
        XOR R4,R5
        MOV R15,[R0+]
        MOV R14,[R0+]
        MOV R13,[R0+]
        MOV R12,[R0+]
        MOV R9,[R0+]
        MOV R8,[R0+]
        MOV R7,[R0+]
        MOV R6,[R0+]
        RETS
__mlf4r ENDP
DC166_MLF4_PR ENDS

        REGDEF R0-R15
        END
