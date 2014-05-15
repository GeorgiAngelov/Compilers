		.data
newline:	.asciiz "\n"
		.text
<<<<<<< HEAD
		main:
move $a0, $v0
li $v0, 1       # Select print_int syscall
syscall
              la $a0, newline
                li $v0, 4               # Select print_string syscall
                syscall
li $v0, 10
syscall
=======
move $fp $sp
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
		main:
li $v0, 1
sub $sp, $sp, 4
sw $v0, 8($fp)
add $sp, $sp, 8
li $v0, 10
 syscall
		skipto:
li $v0, 5
sub $sp, $sp, 4
sw $v0, 4($fp)
add $sp, $sp, 8
jr $ra #skipto
>>>>>>> fa6b0fa23d66508ae96ce1c1dd5a7cfe69e73620
