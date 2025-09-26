add $sp, $zero, $zero, $zero, 0, 0		        # set $sp = 0
add $sp, $sp, $imm2, $zero, 0, -2		        # adjust stack for 2 items
sw $zero, $sp, $imm1, $s1, 1, 0			        # save $s1
sw $zero, $sp, $imm1, $s0, 0, 0			        # save $s0

lw  $s0, $imm1, $zero, $zero, 0x100, 0          # $s0 = r
mac $s0, $s0, $s0, $zero, 0, 0                  # $s0 = r^2
add $s1, $zero, $zero, $zero, 0, 0              # $s1 = curent pixel

ROW:
srl $t0, $s1, $imm1, $zero, 8, 0                # $t0 = curent pixel/256 = row
bgt $zero, $t0, $imm1, $imm2, 255, END          # done going over all the pixels
sub $t0, $t0, $imm1, $zero, 128, 0              # $t0 = row - 128 
bge $zero, $t0, $zero, $imm1, COL, 0            # if $t0>=0, COL
mac $t0, $t0, $imm1, $zero, -1, 0               # $t2 = $t2*(-1)

COL:
and $t1, $s1, $imm1, $imm2, 255, 255            # $t1 = curent pixel%256 = col
sub $t1, $t1, $imm1, $zero, 128, 0              # $t1 = col - 128 
bge $zero, $t1, $zero, $imm1, DIST, 0           # if $t1>=0, DIST
mac $t1, $t1, $imm1, $zero, -1, 0               # $t1 = $t1*(-1)

DIST:
mac $t0, $t0, $t0, $zero, 0, 0                  # $t0 = $t0^2
mac $t1, $t1, $t1, $zero, 0, 0                  # $t1 = $t1^2
add $t1, $t0, $t1, $zero, 0, 0                  # $t1 = (dist from center)^2
bgt $zero, $t1, $s0, $imm1, L1, 0               # pixel not in range, L1

out $zero, $imm1, $zero, $s1, 20, 0             # IOregister[20] = curent pixel
out $zero, $imm1, $zero, $imm2, 21, 255         # change color to white
out $zero, $imm1, $zero, $imm2, 22, 1           # write pixel to monitor

L1:
add $s1, $s1, $imm1, $zero, 1, 0                # move to the next pixel
beq $zero, $zero, $zero, $imm1, ROW, 0		    # jump to ROW

END:
lw $s0, $sp, $imm1, $zero, 0, 0			        # restore $s0
lw $s1, $sp, $imm1, $zero, 1, 0			        # restore $s1
add $sp, $sp, $imm1, $zero, 2, 0		        # pop 2 items from stack
halt $zero, $zero, $zero, $zero, 0, 0		    #finish

.word 0x100 10