		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
jr $ra
fred:
		fred:
sub $sp, $sp, 4
li $v0, 5
sub $sp, $sp, 4
sw $v0, 8($fp)
lw $v0, 8($fp)
move $v1, $v0
li $v0, 3
seq $v0, $v1, $v0
beq $v0, 0, _lbl0
li $v0, 10
add $sp, $sp, 8
jr $ra #fred
j _lbl1
_lbl0:
_lbl1:
li $v0, 5
add $sp, $sp, 8
jr $ra #fred
jr $ra
main:
		main:
sub $sp, $sp, 4
li $v0, 15
sub $sp, $sp, 4
sw $v0, 16($fp)
li $v0, 0
add $sp, $sp, 8
li $v0, 10
 syscall
