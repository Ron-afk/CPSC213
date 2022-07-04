.pos 0x100
    ld $a, r0           # r0 = address of a
    ld $p, r1           # r1 = address of p
    ld $b, r2           # r2 = address of b[0]
    ld $3, r3           # r3 = 3
    st r3, (r0)         # a = r3 = 3
    st r0, (r1)         # p = address of a
    ld (r1), r4         # r4 = p = address of a
    ld (r4), r5         # r5 = *p
    dec r5              # r5 = *p - 1
    st r5, (r4)         # *p = *p - 1
    st r2, (r1)         # p = &b[0]
    ld (r1), r6           # r6 = p
    inca r6              # r6++
    st r6, (r1)         # p = p++
    ld (r0), r7         # r7 = a
    ld (r2, r7,4), r4   # r4 = b[a]
    st r4,(r6,r7,4)     # p[a] = b[a]
    ld (r2), r2         # r2 = b[0]
    ld (r1), r6         # r6 = p
    ld $3, r7           # r7 = 3
    shl $2, r7           # r7 = r7 * 4
    add r7, r6          # r6 = p+3
    st r2, (r6)         # *(p+3) = b[0]
    halt


.pos 0x1000
a: .long 0      # a
p: .long 0      # p

.pos 0x2000
b:  .long 0     # b[0]
    .long 1     # b[1]
    .long 2     # b[2]
    .long 3     # b[3]
    .long 4     # b[4]
