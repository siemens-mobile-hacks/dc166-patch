$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$NONSEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_CIF44

DC166_CIF44_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __cif44r
        PUBLIC __cuf44r
        EXTERN _dc166_sf_from_i32:FAR
        EXTERN _dc166_sf_from_u32:FAR

__cif44r PROC FAR
        MOV [-R0],R6
        MOV [-R0],R7
        MOV [-R0],R8
        MOV [-R0],R9
        MOV [-R0],R12
        MOV [-R0],R13
        MOV [-R0],R14
        MOV [-R0],R15
        MOV R12,R4
        MOV R13,R5
        CALLS SEG _dc166_sf_from_i32,_dc166_sf_from_i32
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
__cif44r ENDP

__cuf44r PROC FAR
        MOV [-R0],R6
        MOV [-R0],R7
        MOV [-R0],R8
        MOV [-R0],R9
        MOV [-R0],R12
        MOV [-R0],R13
        MOV [-R0],R14
        MOV [-R0],R15
        MOV R12,R4
        MOV R13,R5
        CALLS SEG _dc166_sf_from_u32,_dc166_sf_from_u32
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
__cuf44r ENDP
DC166_CIF44_PR ENDS

        REGDEF R0-R15
        END
