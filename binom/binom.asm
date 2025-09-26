	sll $sp, $imm1, $imm2, $zero, 1, 11		        # set $sp = 1 << 11 = 2048
        lw $a0, $zero, $imm2, $zero, 0, 0x100		# get n from address 0x100
        lw $a1, $zero, $imm2, $zero, 0, 0x101	    # get k from address 0x101
    	jal $ra, $zero, $zero, $imm2, 0, BIN		# calc $v0 = binom(n,k)
    	sw $zero, $zero, $imm2, $v0, 0, 0x102		# store binom(n,k) in 0x102
	    halt $zero, $zero, $zero, $zero, 0, 0		# halt
BIN: 
        add $sp, $sp, $imm2, $zero, 0, -4		# adjust stack for 4 items
        sw $zero, $sp, $imm2, $s0, 0, 3			# save $s0
	    sw $zero, $sp, $imm2, $ra, 0, 2			# save return address
        sw $zero, $sp, $imm2, $a1, 0, 1			# save argument
    	sw $zero, $sp, $imm2, $a0, 0, 0			# save argument
	    beq $zero, $a0, $a1, $imm2, 0, L1		# jump to L1 if n==k
        beq $zero, $zero, $a1, $imm2, 0, L1		# jump to L1 if k==0
        beq $zero, $zero, $zero, $imm2, 0, L2           #jump to L2
L1:
        add $v0, $zero, $imm2, $zero, 0, 1		#if(k==0||n==k) binom(n,k) = 1
        beq $zero, $zero, $zero, $imm2, 0, L3           #jump to L3
L2:
        sub $a0, $a0, $imm2, $zero, 0, 1		# calculate n - 1
        jal $ra, $zero, $zero, $imm2, 0, BIN		# calc $v0=binom(n-1,k)
        add $s0, $v0, $zero, $zero, 0, 0		# $s0 = binom(n-1,k)
        sub $a1, $a1, $imm2, $zero, 0, 1		# calculate k - 1
        jal $ra, $zero, $zero, $imm2, 0, BIN		# calc $v0=binom(n-1,k-1)
        add $v0, $v0, $s0, $zero, 0, 0			# $v0 = binom(n-1,k) + binom(n-1,k-1)
        lw $a0, $sp, $imm2, $zero, 0, 0			# restore $a0
        lw $a1, $sp, $imm2, $zero, 0, 1			# restore $a1
    	lw $ra, $sp, $imm2, $zero, 0, 2			# restore $ra
    	lw $s0, $sp, $imm2, $zero, 0, 3			# restore $s0
L3:
    	add $sp, $sp, $imm2, $zero, 0, 4		# pop 4 items from stack
        beq $zero, $zero, $zero, $ra, 0, 0		# and return
		.word 0x100 1
		.word 0x101 1 
  
