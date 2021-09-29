.globl main
.globl foo

.data
m1: .word 6
m2: .word 5

.text
main:
	la $t1, m1
	lw $t0, 0($t1)
	la $t1, m2
	lw $t1, 0($t1)
	add $t1, $t0, $t1
foo:
	la $t1, m1
	lw $t0, 0($t1)
	la $t1, m2
	lw $t1, 0($t1)
	div $t1, $t0, $t1
