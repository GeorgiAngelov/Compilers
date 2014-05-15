		.data
newline:	.asciiz "\n"
		.text
		main:
move $a0, $v0
li $v0, 1       # Select print_int syscall
syscall
              la $a0, newline
                li $v0, 4               # Select print_string syscall
                syscall
li $v0, 10
syscall