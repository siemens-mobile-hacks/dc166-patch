$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME DC166_LOADLH

DC166_LOADLH_PR SECTION CODE WORD PUBLIC 'SHAREDRTLIB'
        PUBLIC __loadlh

; Load a 32-bit value through a Huge segment:offset pointer in R5:R4.
; R5=ffffh is an internal compiler marker for a Near pointer in R4.
__loadlh PROC FAR
        CMP R5,#0ffffh
        JMPR cc_NE,__loadlh_huge
        MOV R10,[R4]
        MOV R11,[R4+#02h]
        RETS

__loadlh_huge:
        EXTS R5,#01h
        MOV R10,[R4]
        ADD R4,#02h
        ADDC R5,#00h
        EXTS R5,#01h
        MOV R11,[R4]
        RETS
__loadlh ENDP
DC166_LOADLH_PR ENDS

        REGDEF R0-R15
        END
