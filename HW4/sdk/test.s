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
move $t0, $v0
li $t1, 1
benz $t0, $t1, _lbl0
li $t2, 2
li $t3, 2
add $v0, $t2, $t3
li $t1, 0
beq $t0, $t1, _lbl1
_lbl0:
_lbl1:
_lbl2:While Condition Here
li $t2, 5
li $t3, 5
add $v0, $t2, $t3
j _lbl2
move $a0, $v0
li $v0, 1       # Select print_int syscall
syscall
              la $a0, newline
                li $v0, 4               # Select print_string syscall
                syscall
li $v0, 10
syscall
