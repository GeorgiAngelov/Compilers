		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 244
sub $sp, $sp, 4
sw $v0, 8($fp)
		fred:
sub $sp, $sp, 4
li $v0, 5
sub $sp, $sp, 4
sw $v0, 16($fp)
li $v0, 10
add $sp, $sp, 12
jr $ra #fred
		main:
sub $sp, $sp, 4
li $v0, 15
sub $sp, $sp, 4
sw $v0, 24($fp)
li $v0, 0
add $sp, $sp, 16
li $v0, 10
 syscall
