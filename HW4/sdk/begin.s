		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
li $v0, 3
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sub $sp, $sp, 4
sw $v0, 4($fp)
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sub $sp, $sp, 4
sw $v0, 8($fp)
sub $sp, $sp, 4
li $v0, 0
sub $sp, $sp, 4
sw $v0, 16($fp)
li $v0 2
sw $v0, 12($fp)
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
li $v0, 4
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 12($fp)
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0, 6
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 7
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
sw $v0, 8($fp)
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
li $v0, 8
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
sub $sp, $sp, 4
li $v0, 4
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
div $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 6
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 12($fp)
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
div $v0, $v1, $v0
sw $v0, 0($sp)
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0, 3
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0, 7
sw $v0, 0($sp)
lw $v0, 8($fp)
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 6
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 4
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
sw $v0, 4($fp)
_lbl0:
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 20
lw $v1, 0($sp)
add $sp, $sp, 4
slt $v0, $v1, $v0
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
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
sub $sp, $sp, 4
li $v0, 5
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
sgt $v0, $v1, $v0
beq $v0, 0, _lbl2
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 3
lw $v1, 0($sp)
add $sp, $sp, 4
slt $v0, $v1, $v0
beq $v0, 0, _lbl4
sub $sp, $sp, 4
li $v0, 1
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 12($fp)
j _lbl5
_lbl4:
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 12($fp)
_lbl5:
j _lbl3
_lbl2:
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 12($fp)
_lbl3:
j _lbl0
_lbl1:
li $v0 10
sw $v0, 8($fp)
li $v0 100
sw $v0, 12($fp)
li $v0, 0
sw $v0, 4($fp)
_lbl6:
sub $sp, $sp, 4
lw $v0, 8($fp)
sw $v0, 0($sp)
lw $v0, 12($fp)
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
lw $v1, 4($fp)
bge $v1, $v0, _lbl7
sub $sp, $sp, 4
lw $v0, 12($fp)
sw $v0, 0($sp)
lw $v0, 4($fp)
lw $v1, 0($sp)
add $sp, $sp, 4
sub $v0, $v1, $v0
sw $v0, 12($fp)
lw $v0, 4($fp)
addi $v0, $v0, 1
sw $v0, 4($fp)
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