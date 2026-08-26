$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOEXPANDREGBANK
$MODEL(LARGE)

        NAME    RUNTIME_LINK_ALL_REFERENCES

RUNTIME_LINK_ALL_PR SECTION CODE WORD PUBLIC 'CPROGRAM'
        PUBLIC _runtime_link_all_references
_runtime_link_all_references PROC FAR
        CALLS SEG __adf4r,__adf4r
        CALLS SEG __sbf4r,__sbf4r
        CALLS SEG __adf8r,__adf8r
        CALLS SEG __sbf8r,__sbf8r
        CALLS SEG __cff48r,__cff48r
        CALLS SEG __cff84r,__cff84r
        CALLS SEG __cfi42r,__cfi42r
        CALLS SEG __cfi44r,__cfi44r
        CALLS SEG __cfi82r,__cfi82r
        CALLS SEG __cfi84r,__cfi84r
        CALLS SEG __cfu42r,__cfu42r
        CALLS SEG __cfu44r,__cfu44r
        CALLS SEG __cfu82r,__cfu82r
        CALLS SEG __cfu84r,__cfu84r
        CALLS SEG __cif24r,__cif24r
        CALLS SEG __cuf24r,__cuf24r
        CALLS SEG __cif28r,__cif28r
        CALLS SEG __cuf28r,__cuf28r
        CALLS SEG __cif44r,__cif44r
        CALLS SEG __cuf44r,__cuf44r
        CALLS SEG __cif48r,__cif48r
        CALLS SEG __cuf48r,__cuf48r
        CALLS SEG __cmf4r,__cmf4r
        CALLS SEG __cmf8r,__cmf8r
        CALLS SEG __mlf4r,__mlf4r
        CALLS SEG __mlf8r,__mlf8r
        CALLS SEG __dvf4r,__dvf4r
        CALLS SEG __dvf8r,__dvf8r
        CALLS SEG __ngf8r,__ngf8r
        CALLS SEG __ld0f8r,__ld0f8r
        CALLS SEG __get1f4r,__get1f4r
        CALLS SEG __get2f4r,__get2f4r
        CALLS SEG __get1f8r,__get1f8r
        CALLS SEG __get2f8r,__get2f8r
        CALLS SEG __putf4r,__putf4r
        CALLS SEG __putf8r,__putf8r
        CALLS SEG __load8n,__load8n
        CALLS SEG __ldnoal8n,__ldnoal8n
        CALLS SEG __store8n,__store8n
        CALLS SEG __load8f,__load8f
        CALLS SEG __ldnoal8f,__ldnoal8f
        CALLS SEG __store8f,__store8f
        CALLS SEG __load8h,__load8h
        CALLS SEG __ldnoal8h,__ldnoal8h
        CALLS SEG __store8h,__store8h
        CALLS SEG __load8np,__load8np
        CALLS SEG __ldnoal8np,__ldnoal8np
        CALLS SEG __store8np,__store8np
        CALLS SEG __load8fp,__load8fp
        CALLS SEG __ldnoal8fp,__ldnoal8fp
        CALLS SEG __store8fp,__store8fp
        CALLS SEG __load8hp,__load8hp
        CALLS SEG __ldnoal8hp,__ldnoal8hp
        CALLS SEG __store8hp,__store8hp

        CALLS SEG __camuln,__camuln
        CALLS SEG __casdiln,__casdiln
        CALLS SEG __caslln,__caslln
        CALLS SEG __casmoln,__casmoln
        CALLS SEG __cassrln,__cassrln
        CALLS SEG __caudiln,__caudiln
        CALLS SEG __caumoln,__caumoln
        CALLS SEG __causrln,__causrln
        CALLS SEG __camulf,__camulf
        CALLS SEG __casdilf,__casdilf
        CALLS SEG __casllf,__casllf
        CALLS SEG __casmolf,__casmolf
        CALLS SEG __cassrlf,__cassrlf
        CALLS SEG __caudilf,__caudilf
        CALLS SEG __caumolf,__caumolf
        CALLS SEG __causrlf,__causrlf
        CALLS SEG __camulh,__camulh
        CALLS SEG __casdilh,__casdilh
        CALLS SEG __casllh,__casllh
        CALLS SEG __casmolh,__casmolh
        CALLS SEG __cassrlh,__cassrlh
        CALLS SEG __caudilh,__caudilh
        CALLS SEG __caumolh,__caumolh
        CALLS SEG __causrlh,__causrlh
        MOV R1,#SOF __null4
        MOV R1,#SOF __null8
        CALLS SEG __mul,__mul
        CALLS SEG __sdil,__sdil
        CALLS SEG __udil,__udil
        CALLS SEG __smol,__smol
        CALLS SEG __umol,__umol
        CALLS SEG __icall,__icall
        CALLS SEG __uicall,__uicall
        CALLS SEG __loadlh,__loadlh
        CALLS SEG __storelh,__storelh
        CALLS SEG __pinclh,__pinclh
        CALLS SEG __pdeclh,__pdeclh
        CALLS SEG __loadlhp,__loadlhp
        CALLS SEG __storelhp,__storelhp
        CALLS SEG __banksw,__banksw
        CALLS SEG __ubanksw,__ubanksw
        CALLS SEG __cpf,__cpf
        CALLS SEG __cph,__cph
        CALLS SEG __cpnnb,__cpnnb
        CALLS SEG __cpnnw,__cpnnw
        CALLS SEG __cpnfb,__cpnfb
        CALLS SEG __cpnfw,__cpnfw
        CALLS SEG __cpnhb,__cpnhb
        CALLS SEG __cpnhw,__cpnhw
        CALLS SEG __cpfnb,__cpfnb
        CALLS SEG __cpfnw,__cpfnw
        CALLS SEG __cpffb,__cpffb
        CALLS SEG __cpffw,__cpffw
        CALLS SEG __cpfhb,__cpfhb
        CALLS SEG __cpfhw,__cpfhw
        CALLS SEG __cphnb,__cphnb
        CALLS SEG __cphnw,__cphnw
        CALLS SEG __cphfb,__cphfb
        CALLS SEG __cphfw,__cphfw
        CALLS SEG __cphhb,__cphhb
        CALLS SEG __cphhw,__cphhw
        RETS
_runtime_link_all_references ENDP
RUNTIME_LINK_ALL_PR ENDS

        EXTERN __adf4r:FAR
        EXTERN __sbf4r:FAR
        EXTERN __adf8r:FAR
        EXTERN __sbf8r:FAR
        EXTERN __cff48r:FAR
        EXTERN __cff84r:FAR
        EXTERN __cfi42r:FAR
        EXTERN __cfi44r:FAR
        EXTERN __cfi82r:FAR
        EXTERN __cfi84r:FAR
        EXTERN __cfu42r:FAR
        EXTERN __cfu44r:FAR
        EXTERN __cfu82r:FAR
        EXTERN __cfu84r:FAR
        EXTERN __cif24r:FAR
        EXTERN __cuf24r:FAR
        EXTERN __cif28r:FAR
        EXTERN __cuf28r:FAR
        EXTERN __cif44r:FAR
        EXTERN __cuf44r:FAR
        EXTERN __cif48r:FAR
        EXTERN __cuf48r:FAR
        EXTERN __cmf4r:FAR
        EXTERN __cmf8r:FAR
        EXTERN __mlf4r:FAR
        EXTERN __mlf8r:FAR
        EXTERN __dvf4r:FAR
        EXTERN __dvf8r:FAR
        EXTERN __ngf8r:FAR
        EXTERN __ld0f8r:FAR
        EXTERN __get1f4r:FAR
        EXTERN __get2f4r:FAR
        EXTERN __get1f8r:FAR
        EXTERN __get2f8r:FAR
        EXTERN __putf4r:FAR
        EXTERN __putf8r:FAR
        EXTERN __load8n:FAR
        EXTERN __ldnoal8n:FAR
        EXTERN __store8n:FAR
        EXTERN __load8f:FAR
        EXTERN __ldnoal8f:FAR
        EXTERN __store8f:FAR
        EXTERN __load8h:FAR
        EXTERN __ldnoal8h:FAR
        EXTERN __store8h:FAR
        EXTERN __load8np:FAR
        EXTERN __ldnoal8np:FAR
        EXTERN __store8np:FAR
        EXTERN __load8fp:FAR
        EXTERN __ldnoal8fp:FAR
        EXTERN __store8fp:FAR
        EXTERN __load8hp:FAR
        EXTERN __ldnoal8hp:FAR
        EXTERN __store8hp:FAR
        EXTERN __camuln:FAR
        EXTERN __casdiln:FAR
        EXTERN __caslln:FAR
        EXTERN __casmoln:FAR
        EXTERN __cassrln:FAR
        EXTERN __caudiln:FAR
        EXTERN __caumoln:FAR
        EXTERN __causrln:FAR
        EXTERN __camulf:FAR
        EXTERN __casdilf:FAR
        EXTERN __casllf:FAR
        EXTERN __casmolf:FAR
        EXTERN __cassrlf:FAR
        EXTERN __caudilf:FAR
        EXTERN __caumolf:FAR
        EXTERN __causrlf:FAR
        EXTERN __camulh:FAR
        EXTERN __casdilh:FAR
        EXTERN __casllh:FAR
        EXTERN __casmolh:FAR
        EXTERN __cassrlh:FAR
        EXTERN __caudilh:FAR
        EXTERN __caumolh:FAR
        EXTERN __causrlh:FAR
        EXTERN __null4:WORD
        EXTERN __null8:WORD
        EXTERN __mul:FAR
        EXTERN __sdil:FAR
        EXTERN __udil:FAR
        EXTERN __smol:FAR
        EXTERN __umol:FAR
        EXTERN __icall:FAR
        EXTERN __uicall:FAR
        EXTERN __loadlh:FAR
        EXTERN __storelh:FAR
        EXTERN __pinclh:FAR
        EXTERN __pdeclh:FAR
        EXTERN __loadlhp:FAR
        EXTERN __storelhp:FAR
        EXTERN __banksw:FAR
        EXTERN __ubanksw:FAR
        EXTERN __cpf:FAR
        EXTERN __cph:FAR
        EXTERN __cpnnb:FAR
        EXTERN __cpnnw:FAR
        EXTERN __cpnfb:FAR
        EXTERN __cpnfw:FAR
        EXTERN __cpnhb:FAR
        EXTERN __cpnhw:FAR
        EXTERN __cpfnb:FAR
        EXTERN __cpfnw:FAR
        EXTERN __cpffb:FAR
        EXTERN __cpffw:FAR
        EXTERN __cpfhb:FAR
        EXTERN __cpfhw:FAR
        EXTERN __cphnb:FAR
        EXTERN __cphnw:FAR
        EXTERN __cphfb:FAR
        EXTERN __cphfw:FAR
        EXTERN __cphhb:FAR
        EXTERN __cphhw:FAR

        REGDEF R0-R15
        END
