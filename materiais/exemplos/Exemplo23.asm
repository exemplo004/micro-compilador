.data
.globl valor
valor: .word 10
msg: .asciiz "Ola"
.text
.globl main
main:
li $t0, 5
addi $t1, $t0, 10
add $t2, $t0, $t1
la $t3, msg
lw $t4, 0($t3)
sw $t4, valor
.globl fim
fim: add $t5, $t1, $t2