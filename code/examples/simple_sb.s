.globl main

.data
m1: .word 6
m2: .word 5

.text
main:
	la $t2, m1
	lw $t0, 0($t2)
	la $t3, m2
	lw $t1, 0($t3)
	add $t1, $t0, $t1
	sb $t2, 0($t1)
