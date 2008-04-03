.rodata
.globl snesromsize
snesromsize:	.long  33280
.globl snesrom
.balign 32
snesrom:
.incbin "../source/roms/COL15.SMC"

