	.intel_syntax noprefix
	.code32
	.section .text
	.global fp_emitter

	.equ emit_node,       0x00420d70
	.equ record_result,   0x00408a50
	.equ move_result,     0x0044f660
	.equ emit_rt_helper,  0x0040d230
	.equ emit_ir,         0x00425320
	.equ reserve_location,0x00406850
	.equ alloc_stack_owner,0x004042c0
	.equ emit_assign,     0x0042efb0
	.equ emit_compound,   0x0045df50
	.equ finalize_expr,   0x00426970
	.equ emit_call,       0x00428db0
	.equ emit_indirect,   0x004531b0
	.equ emit_comma,      0x0041b580
	.equ emit_deref,      0x004395c0
	.equ transfer_owner,  0x004187e0
	.equ release_owner,   0x00418800
	.equ free_temp_node,  0x00419e50
	.equ fatal_error,     0x0049c460

	/* Classic c166 internal locations, confirmed in the existing emitters. */
	.equ WANT_PAIR,       0x00060040
	.equ REG_R4_R5,       0x000040a8
	.equ REG_R10_R11,     0x00004174
	.equ RESULT_R4_R5,    0x000640a8
	.equ REG_R4_WORD,     0x00001008
	.equ REG_R5_WORD,     0x0000100a
	.equ REG_R10_WORD,    0x00001014
	.equ REG_R11_WORD,    0x00001016
	.equ CONST_VALUE,     0x0000002a
	.equ WANT_DBL_R11,    0x000b0008
	.equ WANT_DBL_R10,    0x000c0008
	.equ WANT_DBL_RESULT, 0x200b0008
	.equ RESULT_DBL_R4,   0x000b1008
	.equ RESULT_DBL_R10,  0x000b1014
	.equ RESULT_DBL_R11,  0x000b1016
	.equ WANT_NEAR_PTR,   0x00010018

	/* FUN_00444010 helper encoding: base id | suffix '4' | suffix 'r'. */
	.equ RTL_ADF4R,       0x0000092a
	.equ RTL_SBF4R,       0x0000092b
	.equ RTL_MLF4R,       0x0000092c
	.equ RTL_DVF4R,       0x0000092d
	.equ RTL_ADF8R,       0x00000a2a
	.equ RTL_SBF8R,       0x00000a2b
	.equ RTL_MLF8R,       0x00000a2c
	.equ RTL_DVF8R,       0x00000a2d
	.equ RTL_NGF8R,       0x00000a2e
	.equ RTL_LOAD8N,      0x0000104e
	.equ RTL_LOAD8F,      0x0000204e
	.equ RTL_LOAD8H,      0x0000404e
	.equ RTL_STORE8N,     0x0000104f
	.equ RTL_STORE8F,     0x0000204f
	.equ RTL_STORE8H,     0x0000404f

fp_emitter:
	.ifdef DEBUG_BREAK
	int3
	.endif
	.ifdef PROBE_TYPE_FLAGS
	mov eax, dword ptr [esp + 4]
	mov eax, dword ptr [eax + 0x18]
	and eax, 0x000fffff
	or eax, 0xde000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_REQUESTED_CLASS
	mov eax, dword ptr [esp + 8]
	and eax, 0x00ffffff
	or eax, 0xdd000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_EXACT_LOCATION
	mov eax, dword ptr [esp + 12]
	and eax, 0x00ffffff
	or eax, 0xdc000000
	mov dword ptr [eax], eax
	.endif
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi

	mov esi, dword ptr [ebp + 8]       /* node */
	mov edi, dword ptr [ebp + 12]      /* requested class */
	mov ebx, dword ptr [ebp + 16]      /* requested exact location */
	mov eax, dword ptr [esi + 0x18]
	and eax, 0x00002c2f
	cmp eax, 4                         /* true single precision only */
	je .Lfloat_dispatch
	cmp eax, 5                         /* ANSI double precision */
	je .Ldouble_dispatch
	jmp .Lunsupported

.Lfloat_dispatch:
	movzx eax, byte ptr [esi]

	cmp al, 0x1a                       /* value leaf */
	je .Lleaf
	cmp al, 0x32                       /* value leaf */
	je .Lleaf
	cmp al, 0x1b                       /* dereference/lvalue */
	je .Lderef
	cmp al, 0x1f                       /* dereference variant */
	je .Lderef
	cmp al, 0x4b                       /* dereference variant */
	je .Lderef
	cmp al, 0x79                       /* return */
	je .Lreturn
	cmp al, 0x52                       /* + */
	je .Ladd
	cmp al, 0x53                       /* - */
	je .Lsub
	cmp al, 0x4f                       /* * */
	je .Lmul
	cmp al, 0x50                       /* / */
	je .Ldiv
	cmp al, 0x46                       /* unary - */
	je .Lneg
	cmp al, 0x67                       /* assignment */
	je .Lassign
	cmp al, 0x6c                       /* assignment variant */
	je .Lassign
	cmp al, 0x20                       /* compound assignment */
	je .Lcompound
	cmp al, 0x6a                       /* direct function call */
	je .Lcall
	cmp al, 0x6b                       /* indirect/function call variant */
	je .Lindirect
	cmp al, 0x2c                       /* comma */
	je .Lcomma
	jmp .Lunsupported

.Ldouble_dispatch:
	movzx eax, byte ptr [esi]
	cmp al, 0x1a                       /* value leaf */
	je .Lleaf
	cmp al, 0x32                       /* value leaf */
	je .Lleaf
	cmp al, 0x1b                       /* dereference/lvalue */
	je .Ldouble_deref
	cmp al, 0x1f                       /* dereference variant */
	je .Ldouble_deref
	cmp al, 0x4b                       /* dereference variant */
	je .Ldouble_deref
	cmp al, 0x79                       /* return */
	je .Ldouble_return
	cmp al, 0x52                       /* + */
	je .Ldouble_add
	cmp al, 0x53                       /* - */
	je .Ldouble_sub
	cmp al, 0x4f                       /* * */
	je .Ldouble_mul
	cmp al, 0x50                       /* / */
	je .Ldouble_div
	cmp al, 0x46                       /* unary - */
	je .Ldouble_neg
	cmp al, 0x67                       /* assignment */
	je .Ldouble_assign
	cmp al, 0x6c                       /* assignment variant */
	je .Ldouble_assign
	cmp al, 0x20                       /* compound assignment */
	je .Ldouble_compound
	cmp al, 0x6a                       /* direct function call */
	je .Ldouble_call
	cmp al, 0x6b                       /* indirect/function call variant */
	je .Lindirect
	cmp al, 0x2c                       /* comma */
	je .Lcomma
	jmp .Lunsupported

.Lleaf:
	test dword ptr [esi + 0x18], 0x100
	jz .Ldone
	push ebx
	push edi
	push esi
	call move_result
	add esp, 12
	jmp .Ldone

.Lderef:
	test edi, 0x00400000
	jnz .Lderef_lvalue
	push ebx
	push edi
	push esi
	call emit_deref
	add esp, 12
	jmp .Ldone

.Lderef_lvalue:
	mov eax, dword ptr [esi + 0x08]
	cmp byte ptr [esi], 0x1f
	jne .Lderef_have_child
	mov eax, dword ptr [esi + 0x0c]
.Lderef_have_child:
	mov ecx, dword ptr [esi + 0x14]
	and ecx, 0x000f0000
	cmp ecx, 0x00010000
	jne .Lderef_lvalue_wide
	or ecx, 0x18
	jmp .Lderef_lvalue_class_ready
.Lderef_lvalue_wide:
	or ecx, 0x40
.Lderef_lvalue_class_ready:
	push ebx
	push ecx
	push eax
	call emit_node
	add esp, 12

	mov eax, dword ptr [esi + 0x08]
	cmp byte ptr [esi], 0x1f
	jne .Lderef_record
	mov eax, dword ptr [esi + 0x0c]
.Lderef_record:
	mov ebx, eax
	mov ecx, dword ptr [ebx + 0x14]
	or ecx, 0x00400000
	push ecx
	push esi
	call record_result
	add esp, 8

	push ebx
	push esi
	call transfer_owner
	add esp, 8
	jmp .Ldone

.Ldouble_deref:
	test edi, 0x00400000
	jnz .Lderef_lvalue
	mov ebx, dword ptr [esi + 0x08]
	cmp byte ptr [esi], 0x1f
	jne .Ldouble_deref_have_addr
	mov ebx, dword ptr [esi + 0x0c]
.Ldouble_deref_have_addr:
	mov ecx, dword ptr [esi + 0x14]
	and ecx, 0x000f0000
	mov edx, REG_R4_WORD
	cmp ecx, 0x00010000
	jne .Ldouble_deref_addr_wide
	or ecx, 0x18
	jmp .Ldouble_deref_addr_exact
.Ldouble_deref_addr_wide:
	or ecx, 0x40
	mov edx, REG_R4_R5
.Ldouble_deref_addr_exact:
	push edx
	push ecx
	push ebx
	call emit_node
	add esp, 12

	mov eax, dword ptr [esi + 0x14]
	and eax, 0x000f0000
	cmp eax, 0x00040000
	je .Ldouble_deref_load_far
	cmp eax, 0x00050000
	je .Ldouble_deref_load_huge
	cmp eax, 0x00080000
	je .Ldouble_deref_load_huge
	push RTL_LOAD8N
	jmp .Ldouble_deref_emit_load
.Ldouble_deref_load_far:
	push RTL_LOAD8F
	jmp .Ldouble_deref_emit_load
.Ldouble_deref_load_huge:
	push RTL_LOAD8H
.Ldouble_deref_emit_load:
	call emit_rt_helper
	add esp, 4
	.ifdef PROBE_DEREF_AFTER_HELPER
	mov eax, 0xd3100001
	mov dword ptr [eax], eax
	.endif

	/* __load8x creates the user-stack block itself; register it without
	   emitting another R0 adjustment. */
	push 1
	push 8
	call alloc_stack_owner
	add esp, 8
	mov dword ptr [esi + 0x34], eax
	.ifdef PROBE_DEREF_AFTER_ALLOC
	mov eax, 0xd3100002
	mov dword ptr [eax], eax
	.endif
	push REG_R10_WORD
	call reserve_location
	add esp, 4
	push RESULT_DBL_R10
	push esi
	call record_result
	add esp, 8
	.ifdef PROBE_DEREF_AFTER_RECORD
	mov eax, 0xd3100003
	mov dword ptr [eax], eax
	.endif

	push 0
	push ebx
	call record_result
	add esp, 8
	push ebx
	call release_owner
	add esp, 4
	jmp .Ldone

.Lreturn:
	push REG_R4_R5
	push WANT_PAIR
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12
	push RESULT_R4_R5
	push esi
	call record_result
	add esp, 8
	jmp .Ldone

.Ldouble_return:
	/* The public Classic ABI returns a double in a user-stack block and
	   publishes that block through R4.  Keep the internal R10-owned block,
	   but require the return expression to expose its pointer in R4 before
	   the generic function epilogue is emitted. */
	push REG_R4_WORD
	push WANT_DBL_RESULT
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12
	.ifdef PROBE_RESULT_LOCATION
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x14]
	and eax, 0x00ffffff
	or eax, 0xdb000000
	mov dword ptr [eax], eax
	.endif
	mov ebx, dword ptr [esi + 0x0c]
	push dword ptr [ebx + 0x14]
	push esi
	call record_result
	add esp, 8
	push ebx
	push esi
	call transfer_owner
	add esp, 8
	jmp .Ldone

.Ladd:
	mov eax, RTL_ADF4R
	jmp .Lbinary
.Lsub:
	mov eax, RTL_SBF4R
	jmp .Lbinary
.Lmul:
	mov eax, RTL_MLF4R
	jmp .Lbinary
.Ldiv:
	mov eax, RTL_DVF4R
	jmp .Lbinary

.Ldouble_add:
	mov eax, RTL_ADF8R
	jmp .Ldouble_binary
.Ldouble_sub:
	mov eax, RTL_SBF8R
	jmp .Ldouble_binary
.Ldouble_mul:
	mov eax, RTL_MLF8R
	jmp .Ldouble_binary
.Ldouble_div:
	mov eax, RTL_DVF8R
	jmp .Ldouble_binary

.Lneg:
	.ifdef NEG_PASS
	push REG_R4_R5
	push WANT_PAIR
	push dword ptr [esi + 0x08]
	call emit_node
	add esp, 12
	push RESULT_R4_R5
	push esi
	call record_result
	add esp, 8
	jmp .Ldone
	.else
	/* Compute 0.0f - operand using the regular subtract helper. */
	push REG_R10_R11
	push WANT_PAIR
	push dword ptr [esi + 0x08]
	call emit_node
	add esp, 12

	push REG_R4_R5
	call reserve_location
	add esp, 4

	push 0
	push CONST_VALUE
	push 0
	push REG_R4_WORD
	push 0
	push 0x2d
	call emit_ir
	add esp, 24

	push 0
	push CONST_VALUE
	push 0
	push REG_R5_WORD
	push 0
	push 0x2d
	call emit_ir
	add esp, 24

	push RTL_SBF4R
	call emit_rt_helper
	add esp, 4

	push RESULT_R4_R5
	push esi
	call record_result
	add esp, 8
	jmp .Ldone
	.endif

.Ldouble_neg:
	push REG_R10_WORD
	push WANT_DBL_R10
	push dword ptr [esi + 0x08]
	call emit_node
	add esp, 12
	push RTL_NGF8R
	call emit_rt_helper
	add esp, 4
	mov ebx, dword ptr [esi + 0x08]
	push dword ptr [ebx + 0x14]
	push esi
	call record_result
	add esp, 8
	push ebx
	push esi
	call transfer_owner
	add esp, 8
	jmp .Ldone

.Lcompound:
	push ebx
	push edi
	push esi
	call emit_compound
	add esp, 12
	test eax, eax
	jnz .Ldone

.Lassign:
	mov eax, dword ptr [esi + 0x0c]
	movzx eax, byte ptr [eax]
	cmp al, 0x67
	je .Lassign_rhs_first
	cmp al, 0x20
	jne .Lassign_emit
.Lassign_rhs_first:
	push 0
	push 0x02000000
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12
.Lassign_emit:
	push edi
	push dword ptr [esi + 0x0c]
	push dword ptr [esi + 0x08]
	call emit_assign
	add esp, 12
	test edi, edi
	jz .Ldone
	push WANT_PAIR
	push esi
	call finalize_expr
	add esp, 8
	jmp .Ldone

.Ldouble_assign:
	mov eax, dword ptr [esi + 0x08]   /* lhs */
	.ifdef PROBE_LHS_OPCODE
	movzx ecx, byte ptr [eax]
	or ecx, 0xda000000
	mov dword ptr [ecx], ecx
	.endif
	.ifdef PROBE_LHS_LOCATION
	mov ecx, dword ptr [eax + 0x14]
	and ecx, 0x00ffffff
	or ecx, 0xd9000000
	mov dword ptr [ecx], ecx
	.endif
	.ifdef PROBE_LHS_CHILD_FLAGS
	mov ecx, dword ptr [eax + 0x08]
	mov ecx, dword ptr [ecx + 0x18]
	and ecx, 0x00ffffff
	or ecx, 0xd8000000
	mov dword ptr [ecx], ecx
	.endif
	.ifdef PROBE_LHS_CHILD_LOCATION
	mov ecx, dword ptr [eax + 0x08]
	mov ecx, dword ptr [ecx + 0x14]
	and ecx, 0x00ffffff
	or ecx, 0xd7000000
	mov dword ptr [ecx], ecx
	.endif
	movzx ecx, byte ptr [eax]
	cmp cl, 0x1b
	je .Ldouble_assign_deref
	cmp cl, 0x1f
	je .Ldouble_assign_deref
	cmp cl, 0x4b
	je .Ldouble_assign_deref
	jmp .Lassign                       /* generic local/global assignment */

.Ldouble_compound:
	/* The compound node stores its destination in +8 and the already-expanded
	   arithmetic expression in +0c.  Pointer destinations need the same
	   address-preserving store sequence as an ordinary double assignment. */
	mov eax, dword ptr [esi + 0x08]
	movzx ecx, byte ptr [eax]
	cmp cl, 0x1b
	je .Ldouble_compound_deref
	cmp cl, 0x1f
	je .Ldouble_compound_deref
	cmp cl, 0x4b
	je .Ldouble_compound_deref
	jmp .Lcompound

.Ldouble_compound_deref:
	mov ebx, eax                       /* compound lhs */

	/* Preserve the destination address once.  Re-emitting the left operand
	   from the expanded RHS would evaluate a pointer expression twice and, for
	   shared AST nodes, also violates the allocator's ownership invariant. */
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_compound_have_addr
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_compound_have_addr:
	mov ecx, dword ptr [ebx + 0x14]
	and ecx, 0x000f0000
	cmp ecx, 0x00010000
	jne .Ldouble_compound_addr_wide
	or ecx, 0x18
	jmp .Ldouble_compound_addr_ready
.Ldouble_compound_addr_wide:
	or ecx, 0x40
.Ldouble_compound_addr_ready:
	push 0
	push ecx
	push eax
	call emit_node
	add esp, 12

	/* The expanded RHS is op(current-value, rhs).  Emit the independent rhs
	   as the older stack block, then load current-value through the preserved
	   pointer as the top/result block. */
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x0c]
	push REG_R11_WORD
	push WANT_DBL_R11
	push eax
	call emit_node
	add esp, 12
	.ifdef PROBE_COMPOUND_AFTER_RHS
	mov eax, 0xda200001
	mov dword ptr [eax], eax
	.endif

	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_compound_move_addr
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_compound_move_addr:
	mov ecx, dword ptr [ebx + 0x14]
	and ecx, 0x000f0000
	mov edx, REG_R4_WORD
	cmp ecx, 0x00010000
	jne .Ldouble_compound_move_wide
	or ecx, 0x18
	jmp .Ldouble_compound_move_ready
.Ldouble_compound_move_wide:
	or ecx, 0x40
	mov edx, REG_R4_R5
.Ldouble_compound_move_ready:
	push edx
	push ecx
	push eax
	call move_result
	add esp, 12

	mov eax, dword ptr [ebx + 0x14]
	and eax, 0x000f0000
	cmp eax, 0x00040000
	je .Ldouble_compound_load_far
	cmp eax, 0x00050000
	je .Ldouble_compound_load_huge
	cmp eax, 0x00080000
	je .Ldouble_compound_load_huge
	push RTL_LOAD8N
	jmp .Ldouble_compound_emit_load
.Ldouble_compound_load_far:
	push RTL_LOAD8F
	jmp .Ldouble_compound_emit_load
.Ldouble_compound_load_huge:
	push RTL_LOAD8H
.Ldouble_compound_emit_load:
	call emit_rt_helper
	add esp, 4

	push 1
	push 8
	call alloc_stack_owner
	add esp, 8
	mov ecx, dword ptr [esi + 0x0c]
	mov dword ptr [ecx + 0x34], eax
	push REG_R10_WORD
	call reserve_location
	add esp, 4
	mov ecx, dword ptr [esi + 0x0c]
	push RESULT_DBL_R10
	push ecx
	call record_result
	add esp, 8
	.ifdef PROBE_COMPOUND_AFTER_LOAD
	mov eax, 0xda200002
	mov dword ptr [eax], eax
	.endif

	/* The temporary left child is an lvalue (opcode 0x4b), so its location
	   deliberately remains the address-class value.  The loaded stack block
	   therefore belongs directly to the persistent expanded-RHS node above. */
	mov eax, dword ptr [esi + 0x0c]
	push dword ptr [eax + 0x08]
	call free_temp_node
	add esp, 4

	mov eax, dword ptr [esi + 0x0c]
	movzx eax, byte ptr [eax]
	cmp al, 0x52
	je .Ldouble_compound_add
	cmp al, 0x53
	je .Ldouble_compound_sub
	cmp al, 0x4f
	je .Ldouble_compound_mul
	cmp al, 0x50
	je .Ldouble_compound_div
	jmp .Lunsupported
.Ldouble_compound_add:
	mov eax, RTL_ADF8R
	jmp .Ldouble_compound_emit_op
.Ldouble_compound_sub:
	mov eax, RTL_SBF8R
	jmp .Ldouble_compound_emit_op
.Ldouble_compound_mul:
	mov eax, RTL_MLF8R
	jmp .Ldouble_compound_emit_op
.Ldouble_compound_div:
	mov eax, RTL_DVF8R
.Ldouble_compound_emit_op:
	push eax
	call emit_rt_helper
	add esp, 4
	.ifdef PROBE_COMPOUND_AFTER_OP
	mov eax, 0xda200003
	mov dword ptr [eax], eax
	.endif

	/* Mirror the generic compound emitter: the independent/right temporary is
	   retired only after the operation consumed its older stack block. */
	mov eax, dword ptr [esi + 0x0c]
	push dword ptr [eax + 0x0c]
	call free_temp_node
	add esp, 4
	.ifdef PROBE_COMPOUND_AFTER_TRANSFER
	mov eax, 0xda200004
	mov dword ptr [eax], eax
	.endif

	/* The shared tail moves the preserved address back to R4/R5, emits the
	   matching __store8* helper, and propagates/reclaims the assignment value. */
	jmp .Ldouble_assign_move_addr

.Ldouble_assign_deref:
	mov ebx, eax                       /* lhs */
	/* Materialize the destination address before the FP helper calls.  The
	   allocator will keep it in a preserved register (normally R12/R13). */
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_assign_have_addr
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_assign_have_addr:
	mov ecx, dword ptr [ebx + 0x14]
	and ecx, 0x000f0000
	cmp ecx, 0x00010000
	jne .Ldouble_assign_addr_wide
	or ecx, 0x18
	jmp .Ldouble_assign_addr_class_ready
.Ldouble_assign_addr_wide:
	or ecx, 0x40
.Ldouble_assign_addr_class_ready:
	push 0
	push ecx
	push eax
	call emit_node
	add esp, 12
	.ifdef PROBE_ASSIGN_AFTER_ADDR
	mov eax, 0xda100002
	mov dword ptr [eax], eax
	.endif

	/* Materialize the assigned value at the top of the user stack. */
	push REG_R10_WORD
	push WANT_DBL_R10
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12
	.ifdef PROBE_ASSIGN_AFTER_RHS
	mov eax, 0xda100001
	mov dword ptr [eax], eax
	.endif

	/* Move the preserved destination address into the store ABI registers. */
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_assign_move_addr
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_assign_move_addr:
	mov ecx, dword ptr [ebx + 0x14]
	and ecx, 0x000f0000
	mov edx, REG_R4_WORD
	cmp ecx, 0x00010000
	jne .Ldouble_assign_move_wide
	or ecx, 0x18
	jmp .Ldouble_assign_move_exact
.Ldouble_assign_move_wide:
	or ecx, 0x40
	mov edx, REG_R4_R5
.Ldouble_assign_move_exact:
	push edx
	push ecx
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne 1f
	mov eax, dword ptr [ebx + 0x0c]
1:
	push eax
	call move_result
	add esp, 12

	mov eax, dword ptr [ebx + 0x14]
	and eax, 0x000f0000
	cmp eax, 0x00040000
	je .Ldouble_assign_store_far
	cmp eax, 0x00050000
	je .Ldouble_assign_store_huge
	cmp eax, 0x00080000
	je .Ldouble_assign_store_huge
	push RTL_STORE8N
	jmp .Ldouble_assign_emit_store
.Ldouble_assign_store_far:
	push RTL_STORE8F
	jmp .Ldouble_assign_emit_store
.Ldouble_assign_store_huge:
	push RTL_STORE8H
.Ldouble_assign_emit_store:
	call emit_rt_helper
	add esp, 4
	.ifdef PROBE_ASSIGN_AFTER_STORE
	mov eax, 0xda100003
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_ASSIGN_ADDR_OWNER
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne 1f
	mov eax, dword ptr [ebx + 0x0c]
1:
	mov eax, dword ptr [eax + 0x34]
	and eax, 0x00ffffff
	or eax, 0xd5000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_ASSIGN_RHS_OWNER
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x34]
	and eax, 0x00ffffff
	or eax, 0xd4000000
	mov dword ptr [eax], eax
	.endif

	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_assign_release_addr
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_assign_release_addr:
	cmp dword ptr [eax + 0x14], 0
	je .Ldouble_assign_release_addr_owner_prep
	push 0
	push eax
	call record_result
	add esp, 8

.Ldouble_assign_release_addr_owner_prep:
	mov eax, dword ptr [ebx + 0x08]
	cmp byte ptr [ebx], 0x1f
	jne .Ldouble_assign_release_addr_owner
	mov eax, dword ptr [ebx + 0x0c]
.Ldouble_assign_release_addr_owner:
	push eax
	call release_owner
	add esp, 4
	.ifdef PROBE_ASSIGN_AFTER_RELEASE_ADDR
	mov eax, 0xda300001
	mov dword ptr [eax], eax
	.endif

	test edi, edi
	jnz .Ldouble_assign_keep_value
	push dword ptr [esi + 0x0c]
	call release_owner
	add esp, 4
	.ifdef PROBE_ASSIGN_AFTER_RELEASE_RHS
	mov eax, 0xda300002
	mov dword ptr [eax], eax
	.endif
	jmp .Ldone

.Ldouble_assign_keep_value:
	/* The assignment value is the already materialized RHS block.  The store
	   helper may clobber R10, so ask the regular mover to restore its pointer,
	   then hand both the location and stack owner to the assignment node. */
	push REG_R10_WORD
	push WANT_DBL_RESULT
	push dword ptr [esi + 0x0c]
	call move_result
	add esp, 12
	mov eax, dword ptr [esi + 0x0c]
	push dword ptr [eax + 0x14]
	push esi
	call record_result
	add esp, 8
	push dword ptr [esi + 0x0c]
	push esi
	call transfer_owner
	add esp, 8
	jmp .Ldone

.Lcall:
	push esi
	call emit_call
	add esp, 4
	push REG_R4_R5
	call reserve_location
	add esp, 4
	push RESULT_R4_R5
	push esi
	call record_result
	add esp, 8
	jmp .Ldone

.Ldouble_call:
	push esi
	call emit_call
	add esp, 4
	.ifdef PROBE_CALL_AFTER_GENERIC
	mov eax, 0xd2100001
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_CALL_RIGHT_OPCODE
	mov eax, dword ptr [esi + 0x0c]
	movzx eax, byte ptr [eax]
	or eax, 0xd2000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_CALL_RIGHT_OWNER
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x34]
	and eax, 0x00ffffff
	or eax, 0xd1000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_CALL_ARG_OPCODE
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x08]
	movzx eax, byte ptr [eax]
	or eax, 0xd0000000
	mov dword ptr [eax], eax
	.endif
	.ifdef PROBE_CALL_ARG_OWNER
	mov eax, dword ptr [esi + 0x0c]
	mov eax, dword ptr [eax + 0x08]
	mov eax, dword ptr [eax + 0x34]
	and eax, 0x00ffffff
	or eax, 0xcf000000
	mov dword ptr [eax], eax
	.endif
	/* Classic callees publish the user-stack return block through R4.  Record
	   that ABI location first, then let the ordinary mover satisfy the
	   expression's requested internal location (normally R10). */
	push REG_R4_WORD
	call reserve_location
	add esp, 4
	push RESULT_DBL_R4
	push esi
	call record_result
	add esp, 8
	test edi, edi
	jz .Ldone
	push ebx
	push edi
	push esi
	call move_result
	add esp, 12
	jmp .Ldone

.Lindirect:
	push ebx
	push edi
	push esi
	call emit_indirect
	add esp, 12
	jmp .Ldone

.Lcomma:
	push ebx
	push edi
	push esi
	call emit_comma
	add esp, 12
	jmp .Ldone

.Lbinary:
	push eax                            /* preserve helper id across recursive calls */
	push REG_R4_R5
	push WANT_PAIR
	push dword ptr [esi + 0x08]
	call emit_node
	add esp, 12

	push REG_R10_R11
	push WANT_PAIR
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12

	/* Evaluation of operand 2 may have displaced operand 1. */
	mov ecx, dword ptr [esi + 0x08]
	cmp dword ptr [ecx + 0x14], REG_R4_R5
	je .Lleft_ready
	push REG_R4_R5
	push WANT_PAIR
	push ecx
	call move_result
	add esp, 12
.Lleft_ready:
	pop eax
	push eax
	call emit_rt_helper
	add esp, 4

	push RESULT_R4_R5
	push esi
	call record_result
	add esp, 8
	jmp .Ldone

.Ldouble_binary:
	push eax                            /* preserve helper id */
	/* Evaluate the second operand first.  Its older user-stack block is
	   addressed by R11; the first operand/result is the top block via R10. */
	push REG_R11_WORD
	push WANT_DBL_R11
	push dword ptr [esi + 0x0c]
	call emit_node
	add esp, 12

	push REG_R10_WORD
	push WANT_DBL_R10
	push dword ptr [esi + 0x08]
	call emit_node
	add esp, 12

	/* A nested first operand may displace the saved second-operand pointer
	   from R11 to R12 or beyond.  Arithmetic helpers preserve R10/R11, so
	   restore the pointer explicitly before invoking the outer helper. */
	mov ecx, dword ptr [esi + 0x0c]
	cmp dword ptr [ecx + 0x14], RESULT_DBL_R11
	je .Ldouble_right_ready
	push REG_R11_WORD
	push WANT_DBL_R11
	push ecx
	call move_result
	add esp, 12
.Ldouble_right_ready:

	pop eax
	push eax
	call emit_rt_helper
	add esp, 4

	/* The helper leaves its result in the first operand's top block. */
	mov ebx, dword ptr [esi + 0x08]
	push dword ptr [ebx + 0x14]
	push esi
	call record_result
	add esp, 8
	push ebx
	push esi
	call transfer_owner
	add esp, 8

	/* Mark the older second-operand block free.  The allocator coalesces it
	   when the result owner is eventually released. */
	push dword ptr [esi + 0x0c]
	call release_owner
	add esp, 4
	jmp .Ldone

.Lunsupported:
	/* Keep an explicit failure for operations not implemented by this MVP. */
	push 0x005112c8
	push 0x0000220f
	call fatal_error
	add esp, 8

.Ldone:
	pop edi
	pop esi
	pop ebx
	leave
	ret
