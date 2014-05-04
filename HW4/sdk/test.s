		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0 2
sw $v0, 8($fp)
li $v0 0
sw $v0, 4($fp)
sub $sp, $sp, 4
_lbl0:
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 5
lw $v1, 0($sp)
add $sp, $sp, 4
slt $v0, $v1, $v0
sw $v0, 0($sp)
li $v1, 0
beq $v0, $v1, _lbl1
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 4($fp)
j _lbl0
_lbl1:
add $sp, $sp, 4
li $v0, 0
lw $a0, 4($fp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
lw $a0, 8($fp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
li $v0, 10
syscall