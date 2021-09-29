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
	li $t2, 1
	li $t3, 0
lab:
	add $t3, $t3, $t0
	sub $t4, $t2, $t1
	addi $t2, $t2, 1
	bltz $t4, lab
	la $t4, m3
	sw $t3, 0($t4)
	nop
