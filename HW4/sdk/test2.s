		.data
newline:	.asciiz "\n"
		.text
		g_global:
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($gp)
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($gp)
jal main_start
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
add $sp, $sp, 0
jr $ra #fred
		sally:
li $v0, 10
add $sp, $sp, 0
jr $ra #sally
		george:
_lbl0:
lw $v0, 4($gp)
move $v1, $v0
li $v0, 400
slt $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl1
sub $sp, $sp, 4
lw $v0, 4($gp)
sw $v0, 4($sp)
sub $sp, $sp, 12
sw $ra, -8($fp)
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
jal fred
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
lw $v1, 4($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
# mode is fred
sw $v0, 4($gp)
j _lbl0
_lbl1:
li $v0, 5
add $sp, $sp, 0
jr $ra #george
		main:
move $gp $sp
move $fp $sp
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
jal fred
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
# mode is fred
sw $v0, 4($gp)
sub $sp, $sp, 12
sw $ra, -8($fp)
sw $fp, -4($sp)
sw $sp, 0($sp)
move $fp, $sp
jal george
lw $sp, 0($fp)
lw $fp, -4($fp)
lw $ra, -8($fp)
add $sp, $sp, 12
# mode is george
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
