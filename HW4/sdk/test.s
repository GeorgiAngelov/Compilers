		.data
newline:	.asciiz "\n"
		.text
		main:
li $t0, 2
li $t1, 1
li $t2, 3
mul $v0, $t1, $t2
move $t1, $v0
li $t2, 4
li $t3, 5
li $t4, 6
mul $v0, $t3, $t4
move $t3, $v0
add $v0, $t2, $t3
move $t2, $v0
add $v0, $t1, $t2
move $t1, $v0
li $t2, 7
li $t3, 8
add $v0, $t2, $t3
move $t2, $v0
add $v0, $t1, $t2
move $t1, $v0
div $v0, $t1, $t0
_lbl0: While Condition Here
li $t0, 5
li $t1, 5
add $v0, $t0, $t1
j _lbl0
move $a0, $v0
li $v0, 1       # Select print_int syscall
syscall
              la $a0, newline
                li $v0, 4               # Select print_string syscall
                syscall
li $v0, 10
syscall