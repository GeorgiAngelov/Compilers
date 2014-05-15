		.data
newline:	.asciiz "\n"
		.text
move $fp $sp
		fred:
mov $fp, $sp
li $v0, 10
add $sp, $sp, 0
mov $sp, 0($fp)
mov $fp, -4($fp)
add $sp, $sp, 8
jr $ra #fred
		main:
mov $fp, $sp
sub $sp, $sp, 8
sw $fp, -4($sp)
sw $sp, 0($sp)
mov $fp, $sp
jal fred
li $v0, 0
add $sp, $sp, 0
li $v0, 10
 syscall
