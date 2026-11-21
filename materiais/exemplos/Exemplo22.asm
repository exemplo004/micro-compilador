.data
valor: .word 10
.text
main:
li $t0, 5
li $t1, 10
add $t2, $t0, $t1
mflo $t3
mfhi $t4
jr $t5
loop: mflo $t6
mfhi $t7
jr $t8
lw $t3, 0($t2)
sw $t3, valor