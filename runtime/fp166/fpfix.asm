$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(SMALL)

        NAME DC166_FPFIX

DC166_FPFIX_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __dzerfr
        EXTERN __putf8r:FAR

; __cff48r leaves the binary32 sign in R3.0.  Unlike the generic
; positive-zero helper, this path must copy it to the binary64 result.
__dzerfr PROC FAR
        MOV R4,R3
        MOV R1,#0
        MOV R7,#0
        MOV R8,#0
        MOV R9,#0
        MOV R5,#0
        JMPS SEG __putf8r,__putf8r
        RETS
__dzerfr ENDP
DC166_FPFIX_PR ENDS

        REGDEF R0-R15
        END
