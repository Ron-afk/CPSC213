.pos 0x100
    ld $b, r1
    ld (r1), r1
    ld $1,r2
    ld (r1,r2,4), r2
    ld (r1,r2,4), r3
    halt

.pos 0x1000
a:  .long 0x2000
b:  .long 0x2004

.pos 0x2000
    .long 0x6
    .long 0x3
    .long 0x2
    .long 0x4
    .long 0x1
    .long 0x5
    .long 0x5
    .long 0x4
    .long 0x4