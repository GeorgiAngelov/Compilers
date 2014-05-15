		.data
newline:	.asciiz "\n"
		.text
		d_global:
move $gp $sp
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($gp)
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($gp)
jal main_start
		fred:
li $v0, 15
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		main:
jal d_global
		main_start:
sub $sp, $sp, 8
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
jal fred
# mode is fred
sw $v0, 8($gp)
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
