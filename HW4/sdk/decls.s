		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 1
sub $sp, $sp, 4
sw $v0, 8($fp)
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
add $sp, $sp, 8
li $v0, 10
 syscall