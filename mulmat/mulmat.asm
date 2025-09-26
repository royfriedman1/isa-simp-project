    add $t0,$zero,$zero,$zero,0,0              # initialize i=0
LOOPJ:
    add $t1,$zero,$zero,$zero,0,0              # initialize j=0
LOOPK:
    add $t2,$zero,$zero,$zero,0,0              # initialize k=0
    add $a0,$zero,$zero,$zero,0,0              # initialize temp_result=0
MULTIPLY: 
    mac $a1,$imm1,$t0,$t2,4,0                   # $a1 = 4i+k  (i needs to increase in 1 in this loop )
    lw $a1,$a1,$imm1,$zero,256,0                # load value from MEM[256+4i+k] 
    mac $a2,$imm1,$t2,$t1,4,0                   # $a2 = j+4k  (j needs to increase in 4 in this loop )
    lw $a2,$a2,$imm1,$zero,272,0                # load value from MEM[272+j+4k]
    mac $a0,$a1,$a2,$a0,0,0                     # temp_result+= mat1[i][k] * mat2[k][j] 
    add $t2,$t2,$imm1,$zero,1,0                 # k = k+1
    blt $zero,$t2,$imm1,$imm2,4,MULTIPLY        # if k<4 jump to MULTIPLY
    mac $a2,$imm1,$t0,$t1,4,0	                # $a2=4i+j
	sw $a0,$a2,$imm1,$zero,288,0                # mat3[i][j] = $a0
    add $t1,$t1,$imm1,$zero,1,0                 # j=j+1
    blt $zero,$t1,$imm1,$imm2,4,LOOPK           # if j<4 jump to LOOPK
    add $t0,$t0,$imm1,$zero,1,0                 # i=i+1
    blt $zero,$t0,$imm1,$imm2,4,LOOPJ           # if i<4 jump to LOOPJ
    halt $zero,$zero,$zero,$zero, 0, 0		    # halt

    .word 256 1                  # matrix 1 starts at adddress 0x100
	.word 257 2
	.word 258 3
	.word 259 4
	.word 260 5
	.word 261 6
	.word 262 7
	.word 263 8
	.word 264 9
	.word 265 10
	.word 266 11
	.word 267 12
	.word 268 13
	.word 269 14
	.word 270 15
	.word 271 16
	.word 272 1                  # matrix 2 starts at address 0x110 
	.word 273 2
	.word 274 3
	.word 275 4
	.word 276 5
	.word 277 6
	.word 278 7
	.word 279 8
	.word 280 9
	.word 281 10
	.word 282 11
	.word 283 12
	.word 284 13
	.word 285 14
	.word 286 15
	.word 287 16

  # matrix 3 starts at address 0x120