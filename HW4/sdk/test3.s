		.data
newline:	.asciiz "\n"
		.text
		g_global:
move $gp $sp
move $fp $sp
li $v0, 0
sub $sp, $sp, 4
sw $v0, 4($gp)
jal main_start
		fred:
lw $v0, 4($gp)
move $v1, $v0
li $v0, 400
slt $v0, $v1, $v0
beq $v0, 0, _lbl0
lw $v0, 4($gp)
move $v1, $v0
li $v0, 5
add $v0, $v1, $v0
# mode is fred
sw $v0, 4($gp)
sub $sp, $sp, 12
sw $ra, -8($fp)
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
lw $v0, 4($fp)
sub $sp, $sp, 4
sw $v0, 4($fp)
jal fred
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
j _lbl1
_lbl0:
_lbl1:
lw $v0, 4($gp)
move $v1, $v0
li $v0, 2
mul $v0, $v0, $v1
add $sp, $sp, 4
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
li $v0, 6
sub $sp, $sp, 4
sw $v0, 4($fp)
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
