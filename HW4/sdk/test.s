		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
		fred:
li $v0, 10
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		main:
sub $sp, $sp, 8
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
jal fred
li $v0, 0
add $sp, $sp, 0
li $v0, 10
 syscall
