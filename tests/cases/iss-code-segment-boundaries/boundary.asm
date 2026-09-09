$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME DC166_ISS_CODE_SEGMENT_BOUNDARIES

DC166_BOUNDARY_TESTS SECTION CODE WORD PUBLIC 'CPROGRAM'

        PUBLIC  _test_boundary_jmpr
_test_boundary_jmpr PROC FAR
        CALLS   SEG dc166_edge_jmpr,dc166_edge_jmpr
        RETS
_test_boundary_jmpr ENDP

        PUBLIC  _test_boundary_jmpa
_test_boundary_jmpa PROC FAR
        CALLS   SEG dc166_edge_jmpa,dc166_edge_jmpa
        RETS
_test_boundary_jmpa ENDP

        PUBLIC  _test_boundary_jmpi
_test_boundary_jmpi PROC FAR
        MOV     R8,#0004h
        CALLS   SEG dc166_edge_jmpi,dc166_edge_jmpi
        RETS
_test_boundary_jmpi ENDP

        PUBLIC  _test_boundary_jb
_test_boundary_jb PROC FAR
        BSET    PSW.3
        CALLS   SEG dc166_edge_jb,dc166_edge_jb
        RETS
_test_boundary_jb ENDP

DC166_BOUNDARY_TESTS ENDS

DC166_EDGE_JMPR SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_edge_jmpr PROC FAR
        DB      00Dh,002h
dc166_edge_jmpr ENDP
DC166_EDGE_JMPR ENDS

DC166_TARGET_JMPR SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_target_jmpr PROC FAR
        MOV     R4,#01h
        RETS
dc166_target_jmpr ENDP
DC166_TARGET_JMPR ENDS

DC166_WRONG_JMPR SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_wrong_jmpr PROC FAR
        MOV     R4,#00h
        RETS
dc166_wrong_jmpr ENDP
DC166_WRONG_JMPR ENDS

DC166_EDGE_JMPA SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_edge_jmpa PROC FAR
        DB      0EAh,000h,004h,000h
dc166_edge_jmpa ENDP
DC166_EDGE_JMPA ENDS

DC166_TARGET_JMPA SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_target_jmpa PROC FAR
        MOV     R4,#01h
        RETS
dc166_target_jmpa ENDP
DC166_TARGET_JMPA ENDS

DC166_WRONG_JMPA SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_wrong_jmpa PROC FAR
        MOV     R4,#00h
        RETS
dc166_wrong_jmpa ENDP
DC166_WRONG_JMPA ENDS

DC166_EDGE_JMPI SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_edge_jmpi PROC FAR
        JMPI    cc_UC,[R8]
dc166_edge_jmpi ENDP
DC166_EDGE_JMPI ENDS

DC166_TARGET_JMPI SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_target_jmpi PROC FAR
        MOV     R4,#01h
        RETS
dc166_target_jmpi ENDP
DC166_TARGET_JMPI ENDS

DC166_WRONG_JMPI SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_wrong_jmpi PROC FAR
        MOV     R4,#00h
        RETS
dc166_wrong_jmpi ENDP
DC166_WRONG_JMPI ENDS

DC166_EDGE_JB SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_edge_jb PROC FAR
        DB      08Ah,088h,002h,030h
dc166_edge_jb ENDP
DC166_EDGE_JB ENDS

DC166_TARGET_JB SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_target_jb PROC FAR
        MOV     R4,#01h
        RETS
dc166_target_jb ENDP
DC166_TARGET_JB ENDS

DC166_WRONG_JB SECTION CODE WORD PUBLIC 'BOUNDARY'
dc166_wrong_jb PROC FAR
        MOV     R4,#00h
        RETS
dc166_wrong_jb ENDP
DC166_WRONG_JB ENDS

        REGDEF  R0-R15
        END
