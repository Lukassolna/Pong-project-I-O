	 # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.


.global hexasc

.global delay

.global time2string
  
# Please keep the two macros PUSH and POP unchanged
  
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
.data
.text

hexasc:
PUSH $a0
    andi    $a0, $a0, 0xf    #Only 4 least significant bits is needed
    li    $v0, 0x30    #Load v0 with ASCII value for '0'
    ble    $a0, 0x9, skip 
    nop   #skip if a0 is less than or equal to 9
    addi    $v0, $v0, 7    #Add 7 to go to ASCII 'A'-'F'
   
skip:
    add     $v0, $v0, $a0
     POP $a0
    jr    $ra
    nop
delay:
PUSH $ra
PUSH $s0
PUSH $s1
PUSH $s2
move $s0, $a0 # Move input a0 into s0 (a0 is the input)
li $s1, 8580  #value to change

loop:  #while loop
addi $s0, $s0, -1
bgt   $s0,$0 ,continue
nop
j end
nop
continue:
li $s2,0
loop2: # for loop
    addi $s2, $s2, +1
    bne $s2, $s1, loop2
    nop
j loop
nop
end:
POP $s0
POP $s1
POP $s2
POP $ra
jr $ra
nop

time2string:


PUSH $s0
PUSH $s1
move $s0,$a1
move $s1,$a0
andi $t0, $s0, 0xF000 # Ta bort alla bits utom de 4 least significant
srl $t0, $t0, 12   # shift right
move $a0, $t0  # move input till a0 f�r att anv�nda hexasc
PUSH $ra
jal hexasc
POP $ra
nop   #  anropa hexasc
sb $v0, 0($s1) # store v0 (output from hexasc) into memory pointed by a0

# ANDRA 
andi $t0, $s0, 0xF00
srl $t0, $t0, 8
move $a0, $t0
PUSH $ra
jal hexasc
POP $ra
nop
sb $v0, 1($s1)
# :

li $t0,0x3A
sb $t0, 2($s1)



#tredje

andi $t0, $s0, 0xF0
srl $t0, $t0, 4
move $a0, $t0
PUSH $ra
jal hexasc
POP $ra
nop
sb $v0, 3($s1)
#FJR�DE
andi $t0, $s0, 0xF
beq $t0,9,go
move $a0, $t0
PUSH $ra
jal hexasc
POP $ra
nop
sb $v0, 4($s1)
j nullbyte
nop
go:
li $t2,0x4E
sb $t2, 4($s1)
li $t3,0x49
sb $t3, 5($s1)
li $t4,0x4E
sb $t4, 6($s1)
li $t5,0x45
sb $t5, 7($s1)
j gopast

# NULL BYTE
nullbyte:
sb $0, 5($s1)
POP $s0
POP $s1
jr $ra

gopast:
sb $0, 8($s1)
POP $s0
POP $s1
jr $ra
     

