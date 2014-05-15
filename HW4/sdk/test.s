		.data
newline:	.asciiz "\n"
		.text
move $gp $sp
move $fp $sp
		g_global:
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($gp)
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($gp)
jal main_start
		fred:
sub $sp, $sp, 4
lw $v0, 4($gp)
sw $v0, 4($sp)
lw $v0, 8($gp)
lw $v1, 4($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		sally:
sub $sp, $sp, 4
lw $v0, 4($gp)
sw $v0, 4($sp)
lw $v0, 8($gp)
lw $v1, 4($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #sally
		dostuff:
sub $sp, $sp, 8
sw $fp, -4($fp)
sw $sp, 0($fp)
move $fp, $sp
jal sally
# mode is sally
sw $v0, 4($gp)
li $v0, 0
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #dostuff
		main:
jal g_global
		main_start:
sub $sp, $sp, 8
sw $fp, -4($fp)
sw $sp, 0($fp)
move $fp, $sp
jal dostuff
li $v0, 0
lw $a0, 4($gp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
lw $a0, 8($gp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
add $sp, $sp, 8
li $v0, 10
 syscall
