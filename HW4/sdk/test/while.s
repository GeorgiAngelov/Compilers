		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($fp)
li $v0, 3
sub $sp, $sp, 4
sw $v0, 12($fp)
li $v0, 4
sub $sp, $sp, 4
sw $v0, 16($fp)
_lbl0:
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 10
lw $v1, 0($sp)
add $sp, $sp, 4
slt $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl1
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 4($fp)
j _lbl0
_lbl1:
_lbl2:
sub $sp, $sp, 4
lw $v0, 8($fp)
sw $v0, 0($sp)
li $v0, 20
lw $v1, 0($sp)
add $sp, $sp, 4
sle $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl3
sub $sp, $sp, 4
lw $v0, 8($fp)
sw $v0, 0($sp)
li $v0, 3
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 8($fp)
j _lbl2
_lbl3:
_lbl4:
sub $sp, $sp, 4
lw $v0, 16($fp)
sw $v0, 0($sp)
li $v0, 0
lw $v1, 0($sp)
add $sp, $sp, 4
sgt $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl5
sub $sp, $sp, 4
lw $v0, 16($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 16($fp)
j _lbl4
_lbl5:
_lbl6:
sub $sp, $sp, 4
lw $v0, 12($fp)
sw $v0, 0($sp)
li $v0, 15
lw $v1, 0($sp)
add $sp, $sp, 4
sne $v0, $v1, $v0
li $v1, 0
beq $v0, $v1, _lbl7
sub $sp, $sp, 4
lw $v0, 12($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 12($fp)
j _lbl6
_lbl7:
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
lw $a0, 12($fp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
lw $a0, 16($fp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
li $v0, 10
syscall