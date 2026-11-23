.data
.globl msg
msg: .asciiz "Resultado: "
frase: .ascii "abc"
valor: .word 10
lista: .word 1, 2, 3
meio: .half 7
pequeno: .byte 1
buffer: .space 16
.align 2
.text
.globl main
main:
li $t0, 5
li $t1, 10
la $t2, msg
move $t3, $t0
add $t4, $t0, $t1
addu $t4, $t4, $t1
sub $t5, $t1, $t0
subu $t5, $t5, $t0
mul $t6, $t0, $t1
div $t0, $t1
mflo $t7
mfhi $t8
and $t6, $t0, $t1
or $t6, $t0, $t1
xor $t6, $t0, $t1
nor $t6, $t0, $t1
slt $t9, $t0, $t1
addi $t0, $t0, 1
sll $t1, $t0, 2
srl $t1, $t1, 1
lw $t2, 0($t3)
sw $t2, 4($t3)
lb $t2, 8($t3)
sb $t2, 12($t3)
lh $t2, 16($t3)
sh $t2, 20($t3)
lw $t4, valor
loop: beq $t0, $t1, fim
bne $t0, $t1, loop
blt $t0, $t1, loop
ble $t0, $t1, fim
bgt $t0, $t1, loop
bge $t0, $t1, fim
jal funcao
j fim
funcao: jr $t3
nop
fim:
syscall