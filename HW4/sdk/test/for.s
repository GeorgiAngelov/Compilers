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
lw $v0, 8($fp)
sw $v0, 4($fp)
_lbl0:
li $v0, 10
lw $v1, 4($fp)
bge $v1, $v0, _lbl1
sub $sp, $sp, 4
li $v0, 2
sw $v0, 0($sp)
lw $v0, 8($fp)
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 8($fp)
lw $v0, 16($fp)
sw $v0, 12($fp)
_lbl2:
li $v0, 15
lw $v1, 12($fp)
bge $v1, $v0, _lbl3
sub $sp, $sp, 4
lw $v0, 16($fp)
sw $v0, 0($sp)
li $v0, 1
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 16($fp)
lw $v0, 12($fp)
addi $v0, $v0, 1
sw $v0, 12($fp)
j _lbl2
_lbl3:
lw $v0, 4($fp)
addi $v0, $v0, 1
sw $v0, 4($fp)
j _lbl0
_lbl1:
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
add $sp, $sp, 16
li $v0, 10
 syscall