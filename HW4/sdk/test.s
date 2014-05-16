		.data
newline:	.asciiz "\n"
		.text
		g_global:
move $gp $sp
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($gp)
jal main_start
		sally:
li $v0, 10
add $sp, $sp, 0
jr $ra #sally
		fred:
sub $sp, $sp, 12
sw $ra, -8($fp)
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
jal sally
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 4($sp)
lw $v0, 8($fp)
lw $v1, 4($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
add $sp, $sp, 8
jr $ra #fred
		main:
sub $sp, $sp, 4
sw $ra, 4($fp)
jal g_global
		main_start:
lw $ra, 4($fp)
add $sp, $sp, 4
sub $sp, $sp, 12
sw $ra, -8($fp)
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 6
sub $sp, $sp, 4
sw $v0, 8($fp)
jal fred
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
# mode is fred
sw $v0, 4($gp)
li $v0, 0
lw $a0, 4($gp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
add $sp, $sp, 4
li $v0, 10
 syscall
