		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 1
sub $sp, $sp, 4
sw $v0, 8($fp)
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
sub $sp, $sp, 4
lw $v0, 4($fp)
sw $v0, 0($sp)
li $v0, 3
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
sw $v0, 0($sp)
sub $sp, $sp, 4
li $v0, 4
sw $v0, 0($sp)
sub $sp, $sp, 4
lw $v0, 8($fp)
sw $v0, 0($sp)
li $v0, 6
lw $v1, 0($sp)
add $sp, $sp, 4
mul $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
sub $sp, $sp, 4
li $v0, 7
sw $v0, 0($sp)
li $v0, 8
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
li $v0, 2
lw $v1, 0($sp)
add $sp, $sp, 4
div $v0, $v1, $v0
sw $v0, 12($fp)
lw $a0, 12($fp)
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
lw $a0, 4($fp)
li $v0, 1
syscall
la $a0, newline
li $v0, 4
syscall
add $sp, $sp, 12
li $v0, 10
 syscall