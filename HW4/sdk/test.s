		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
		fred:
sub $sp, $sp, 4
<<<<<<< HEAD
=======
li $v0 5
sw $v0, 8($fp)
>>>>>>> 824785b32f6b046c0e62a51c46a3daae446727aa
add $sp, $sp, 8
jr $ra #fred
li $v0, 10
		main:
sub $sp, $sp, 4
sub $sp, $sp, 4
<<<<<<< HEAD
li $v0 15
sw $v0, 0($fp)
_lbl0:
lw $v0, 0($fp)
move $v1, $v0
li $v0, 20
slt $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl1
lw $v0, 0($fp)
move $v1, $v0
li $v0, 1
add $v0, $v1, $v0
sw $v0, 0($fp)
j _lbl0
_lbl1:
add $sp, $sp, 20
=======
add $sp, $sp, 12
>>>>>>> 824785b32f6b046c0e62a51c46a3daae446727aa
li $v0, 10
 syscall
li $v0, 0
