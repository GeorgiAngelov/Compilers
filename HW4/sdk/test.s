		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
		fred:
sub $sp, $sp, 4
add $sp, $sp, 8
jr $ra #fred
li $v0, 10
		main:
sub $sp, $sp, 4
sub $sp, $sp, 4
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
li $v0, 10
 syscall
li $v0, 0
