		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
sub $sp, $sp, 4
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($fp)
sub $sp, $sp, 4
sub $sp, $sp, 4
li $v0, 5
move $v1, $v0
li $v0, 5
mul $v0, $v0, $v1
sw $v0, 0($sp)
li $v0, 4
move $v1, $v0
li $v0, 7
mul $v0, $v0, $v1
move $v1, $v0
li $v0, 10
mul $v0, $v0, $v1
move $v1, $v0
li $v0, 5
div $v0, $v1, $v0
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sw $v0, 0($sp)
lw $v0, 8($fp)
lw $v1, 0($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
sub $sp, $sp, 4
sw $v0, 12($fp)
sub $sp, $sp, 4
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