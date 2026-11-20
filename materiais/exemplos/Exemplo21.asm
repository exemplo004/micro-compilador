.data
valor: .word 10
.text
main:
lw $t0, 0($t1)
sw $t0, 4($t1)
lb $t2, 8($t3)
sb $t2, 12($t3)
lh $t4, 16($t5)
sh $t4, 20($t5)
lw $t6, valor
sw $t6, valor