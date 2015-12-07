	.section ".text"

	.align 4
	.type q_atomic_trylock_int,#function
	.global q_atomic_trylock_int
q_atomic_trylock_int:
        sethi %hi(-2147483648),%o2
        swap [%o0],%o2
        retl
        mov %o2,%o0
        .size q_atomic_trylock_int,.-q_atomic_trylock_int




        .align 4
        .type q_atomic_trylock_ptr,#function
        .global q_atomic_trylock_ptr
q_atomic_trylock_ptr:
        mov -1, %o2
        swap [%o0], %o2
        retl
        mov %o2, %o0
        .size q_atomic_trylock_ptr,.-q_atomic_trylock_ptr




	.align 4
	.type q_atomic_unlock,#function
	.global q_atomic_unlock
q_atomic_unlock:
	stbar
	retl
	st %o1,[%o0]
	.size q_atomic_unlock,.-q_atomic_unlock




	.align 4
	.type q_atomic_set_int,#function
	.global q_atomic_set_int
q_atomic_set_int:
	swap [%o0],%o1
        stbar
	retl
	mov %o1,%o0
	.size q_atomic_set_int,.-q_atomic_set_int




	.align 4
	.type q_atomic_set_ptr,#function
	.global q_atomic_set_ptr
q_atomic_set_ptr:
	swap [%o0],%o1
        stbar
	retl
	mov %o1,%o0
	.size q_atomic_set_ptr,.-q_atomic_set_ptr

