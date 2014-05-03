		.data
newline:	.asciiz "\n"
		.text
		main:
li $t0, 5
move $t1, $v0
add $v0, $t1, $t0
move $t0, $v0
li $t1, 0
_lbl0:beq $t0, $t1, _lbl1
li $t2, 1
li $t3, 2
add $v0, $t2, $t3
addi $t1, 1
j _lbl0
_lbl1:
move $a0, $v0
li $v0, 1       # Select print_int syscall
syscall
              la $a0, newline
                li $v0, 4               # Select print_string syscall
                syscall
li $v0, 10
syscall