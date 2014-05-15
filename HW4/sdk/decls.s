		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
		main:
li $v0, 1
sub $sp, $sp, 4
sw $v0, 8($fp)
add $sp, $sp, 8
li $v0, 10
 syscall
		skipto:
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
add $sp, $sp, 8
jr $ra #skipto
