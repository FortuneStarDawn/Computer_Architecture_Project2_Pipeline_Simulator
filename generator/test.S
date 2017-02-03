		  bne   $0, $1, 0x2
		  lui   $1, 0x7FFF
		  jal   0x0
		  jal   0x6
		  addi  $30, $31, 0x8
		  jr    $30 
		  jal   0x9
		  jal   0x10
		  sw    $30, 0($0)
		  sw    $31, 4($0)
		  lw    $31, 0($0)
		  nop
		  add   $30, $0, $31
		  add   $30, $0, $0
		  add   $30, $30, $30 
		  add   $29, $0, $0
		  bne   $29, $30, 0x0 
		  ori   $1, $1, 0xFFFF
		  add   $0, $1, $1
		  sll   $2, $1, 0x1
		  srl   $3, $2, 0x4
		  sra   $4, $2, 0x4
		  slti  $5, $2, 0xF000
		  sw    $1, (1020)$0
		  lh    $6, (1022)$0
		  lhu   $7, (1022)$0
		  lb    $8, (1021)$0
		  lbu   $9, (1021)$0
		  addiu $1, $0, 0xABCD
		  sb    $1, (1022)$0
		  lw    $1, (1020)$0	  
		  add   $1, $0, $0
		  lui   $1, 0x8000
		  sub   $2, $1, $1
		  add   $1, $1, $1
		  lh    $0, (1023)$0
		  halt
		  halt
		  halt
		  halt
		  halt

/*
0x00000008
0x00000029
0x14010002
0x3C017FFF
0x0C000000
0x0C000006
0x23FE0008
0x03DEF008
0x0C000009
0x0C00000A
0xAC1E0000
0xAC1F0004
0x8C1F0000
0x03E00000
0x001FF020
0x0000F020
0x03DEF020
0x0000E820
0x17BE0000
0x3421FFFF
0x00210020
0x00011040
0x00021902
0x00022103
0x2845F000
0xAC0103FC
0x840603FE
0x940703FE
0x800803FD
0x900903FD
0x2401ABCD
0xA00103FE
0x8C0103FC
0x00000820
0x3C018000
0x00211022
0x00210820
0x840003FF
0xFC210800
0xFC210800
0xFC210800
0xFC210800
0xFC210800
*/