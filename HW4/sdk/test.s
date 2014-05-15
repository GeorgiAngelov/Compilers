		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
		fred:
sub $sp, $sp, 4
li $v0 5
sw $v0, 8($fp)
add $sp, $sp, 8
jr $ra #fred
li $v0, 10
		main:
sub $sp, $sp, 4
sub $sp, $sp, 4
add $sp, $sp, 12
li $v0, 10
 syscall
li $v0, 0
