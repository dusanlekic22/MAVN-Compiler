.globl main

.data
m1: .word 6
m2: .word 5
m3: .word 0

.text
main:
	la $t0, m1
	lw $t0, 0($t0)
	la $t1, m2
	lw $t1, 0($t1)
	li $t1, 1
	li $t1, 0
lab:
	add $t1, $t1, $t0
