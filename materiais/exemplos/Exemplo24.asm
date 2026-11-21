.data
valor: .word 10
.text
.globl main
main:
li $t0, 5
addi $t1, $t0, 10
add $t2, $t0, $t1
jal funcao
j fim
funcao: add $t3, $t1, $t2
j main
loop: jal funcao
lw $t4, 0($t2)
sw $t4, valor
fim: add $t5, $t1, $t2