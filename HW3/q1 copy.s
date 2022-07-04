.pos 0x100
        ld $r, r0           # r0 = address of r[0]
    ld $3, r1           # r1 = 3
    ld (r0,r1,4), r1    # r1 = r[3]
    ld $l, r2           # r2 = address of l
    st r1, 0(r2)        # l = r[3]
    ld 0(r2), r3        # r3 = l
    ld (r0,r3,4), r3    # r3 = r[l]
    st r3, 0(r2)        # l = r[l]
    ld $8, r4           # r4 = 8
    ld $c, r5           # r5 = address of c
    ld $b, r6          # r6 = address of b
    st r6, 0(r5)        # c = &b
    ld 0(r5), r6        # r6 = c
    st r4, 0(r6)        # *c = 8
    ld $5, r2           # r2 = 5
    ld (r0,r2,4), r2    # r2 = r[5]
    shl $2, r2          # r2 = r[5]*4
    add r0, r2          # r2 = address of r[5]
    st r2, 0(r5)        # c = &r[r[5]]
    ld 0(r5), r5        # r5 = c
    ld $4, r1           # r1 = 4
    ld (r0,r1,4),r1     # r1 = r[4]
    ld 0(r5), r7        # r7 = *c
    add r1, r7          # r7 = *c + r[4]
    st r7, 0(r5)        # *c = *c + r[4]
    halt

.pos 0x1000
l:  .long 0     # l
b:  .long 0     # b
c:  .long 0     # c

.pos 0x2000
r:  .long 0     # r[0]
    .long 1     # r[1]
    .long 2     # r[2]
    .long 3     # r[3]
    .long 4     # r[4]
    .long 5     # r[5]
    .long 6     # r[6]
    .long 7     # r[7]
    .long 8     # r[8]
    .long 9     # r[9]