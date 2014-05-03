		.data
newline:	.asciiz "\n"
		.text
		main:
move $fp $sp
sub $sp, $sp, 4
sub $sp, $sp, 4
li $t0, 1
li $t1, 2
add $v0, $t0, $t1
sw $v0, 8($fp)
li $v0 5
sw $v0, 4($fp)
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