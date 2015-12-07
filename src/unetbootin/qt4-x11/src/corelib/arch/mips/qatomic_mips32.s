	.set nobopt
	.set noreorder
	.option pic2
	.text

	.globl	q_atomic_test_and_set_int
	.ent	q_atomic_test_and_set_int
        .set mips2
q_atomic_test_and_set_int:
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_int

	.globl	q_atomic_test_and_set_acquire_int
	.ent	q_atomic_test_and_set_acquire_int
        .set mips2
q_atomic_test_and_set_acquire_int:
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	sync
	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_acquire_int
	
	.globl	q_atomic_test_and_set_release_int
	.ent	q_atomic_test_and_set_release_int
        .set mips2
q_atomic_test_and_set_release_int:
	sync
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_release_int

	.globl	q_atomic_test_and_set_ptr
	.ent	q_atomic_test_and_set_ptr
        .set mips2
q_atomic_test_and_set_ptr:
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_ptr

	.globl	q_atomic_test_and_set_acquire_ptr
	.ent	q_atomic_test_and_set_acquire_ptr
        .set mips2
q_atomic_test_and_set_acquire_ptr:
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	sync
	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_acquire_ptr
	
	.globl	q_atomic_test_and_set_release_ptr
	.ent	q_atomic_test_and_set_release_ptr
        .set mips2
q_atomic_test_and_set_release_ptr:
	sync
1:	ll   $8,0($4)
	bne  $8,$5,2f
	move $2,$6
	sc   $2,0($4)
	beqz $2,1b
	nop
	jr   $31
	nop
2:	jr   $31
	move $2,$0
        .set mips0
	.end	q_atomic_test_and_set_release_ptr
