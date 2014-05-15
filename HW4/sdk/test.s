		.data
newline:	.asciiz "\n"
		.text
move $gp $sp
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 2
sub $sp, $sp, 4
sw $v0, 8($fp)
		fred:
sub $sp, $sp, 4
lw $v0, 8($gp)
sw $v0, 4($sp)
lw $v0, 8($gp)
lw $v1, 4($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
# mode is fred
sw $v0, 8($gp)
li $v0, 0
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		main:
sub $sp, $sp, 4
lw $v0, 0($fp)
sw $v0, 4($sp)
lw $v0, 0($fp)
lw $v1, 4($sp)
add $sp, $sp, 4
add $v0, $v1, $v0
# mode is main
sw $v0, 0($fp)
li $v0, 0
add $sp, $sp, 12
li $v0, 10
 syscall
