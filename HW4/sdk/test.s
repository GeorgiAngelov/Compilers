		.data
newline:	.asciiz "\n"
		.text
move $gp $sp
move $fp $sp
li $v0, 1
sub $sp, $sp, 4
sub $sp, $sp, 4
sw $v0, 4($fp)
li $v0, 2
sub $sp, $sp, 4
sub $sp, $sp, 4
sw $v0, 8($fp)
		fred:
li $v0 15
# mode is fred
sw $v0, 8($gp)
li $v0, 0
add $sp, $sp, 0
lw $sp, 0($fp)
lw $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		main:
li $v0 3
# mode is main
sw $v0, 4($gp)
li $v0, 0
add $sp, $sp, 8
li $v0, 10
 syscall
