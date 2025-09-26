add $t0, $imm1, $zero, $zero, 7, 0           #$t0 = 7, represents the current sector
out $zero, $imm1, $zero, $imm2, 16, 0        #set diskbuffer to address 0 in data mem

L1:
out $zero, $imm1, $zero, $t0, 15, 0          #IORegister[15] = $t0
out $zero, $imm1, $zero, $imm2, 14, 1        #disccmd is read
jal $ra, $zero, $zero, $imm1, WAIT, 0        #jump to wait to check if the reading is finish
add $t1, $t0, $imm1, $zero, 1, 0             #$t1 = $to+1
out $zero, $imm1, $zero, $t1, 15, 0          #IORegister[15] =$t1
out $zero, $imm1, $zero, $imm2, 14, 2        #disccmd is write
jal $ra, $zero, $zero, $imm1, WAIT, 0        #jump to WAIT to check if the writing is finish
beq $zero, $t0, $zero, $imm1, END, 0         #if $t0=0, END
sub $t0, $t0, $imm1, $zero, 1, 0             #$t0=$t0-1
beq $zero, $zero, $zero, $imm1, L1, 0        #not finished, return to L1

WAIT:
in $t2, $imm1, $zero, $zero, 17, 0           #$rd = diskstatus
beq $zero, $t2, $zero, $ra, 0, 0             #if finish, countinu in L1
beq $zero, $zero, $zero, $imm1, WAIT, 0      #not finish, WAIT

END:
halt $zero, $zero, $zero, $zero, 0, 0        #finish