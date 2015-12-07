	.pred.safe_across_calls p1-p5,p16-p63
.text
	.align 16
	.global q_atomic_test_and_set_int#
	.proc q_atomic_test_and_set_int#
q_atomic_test_and_set_int:
	.prologue
	.body
	mov ar.ccv=r33
	;;
	cmpxchg4.acq r34=[r32],r34,ar.ccv
	;;
	cmp4.eq p6, p7 = r33, r34
	;;
	(p6) addl r8 = 1, r0
	(p7) mov r8 = r0
	br.ret.sptk.many b0
	.endp q_atomic_test_and_set_int#
	.align 16
	.global q_atomic_test_and_set_ptr#
	.proc q_atomic_test_and_set_ptr#
q_atomic_test_and_set_ptr:
	.prologue
	.body
	mov ar.ccv=r33
	;;
	cmpxchg8.acq r34=[r32],r34,ar.ccv
	;;
	cmp.eq p6, p7 = r33, r34
	;;
	(p6) addl r8 = 1, r0
	(p7) mov r8 = r0
	br.ret.sptk.many b0
	.endp q_atomic_test_and_set_ptr#
