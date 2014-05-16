		.data
newline:	.asciiz "\n"
		.text
		g_global:
move $gp $sp
move $fp $sp
jal main_start
		main:
sub $sp, $sp, 4
sw $ra, 4($fp)
jal g_global
		main_start:
lw $ra, 4($fp)
add $sp, $sp, 4
li $v0, 0
add $sp, $sp, 4
li $v0, 10
 syscall
