.pos 0x100
    ld $0, r0           # r0 = 0
    ld $tmp, r1         # r1 = address of tmp
    ld $tos, r2         # r2 = address of tos
    st r0, (r1)         # tmp = 0
    st r0, (r2)         # tos = 0
    ld $a, r3           # r3 = address of a[0]
    ld (r3), r4         # r4 = a[0]
    ld (r2), r5         # r5 = tos
    ld $s, r6           # r6 = address of s[0]
    st r4, (r6,r5,4)    # s[tos] = a[0]
    inc r5              # r5++
    st r5, (r2)         # tos = r5 = tos++
    ld $1, r7           # r7 = 1
    ld (r3,r7,4), r4    # r4 = a[1]
    st r4,(r6,r5,4)     # s[tos] = a[1]
    ld $2, r7           # r7 = 2
    ld (r3,r7,4), r4    # r4 = a[2]
    inc r5              # r5++
    st r5, (r2)         # tos = r5 = tos++
    st r4,(r6,r5,4)     # s[tos] = a[2]
    inc r5              # r5++
    st r5, (r2)         # tos = r5 = tos++
    dec r5              # r5--
    st r5, (r2)         # tos = r5 = tos--
    ld (r6,r5,4), r0    # r0 = s[tos]
    st r0, (r1)         # tmp = r0 = s[tos]
    dec r5              # r5--
    st r5, (r2)         # tos = r5 = tos--
    ld (r6,r5,4), r7    # r7 = s[tos]
    add r7, r0          # r0 = tmp + s[tos]
    st r0, (r1)         # tmp = r0
    dec r5              # r5--
    st r5, (r2)         # tos = r5 = tos--
    ld (r6,r5,4), r7    # r7 = s[tos]
    add r7, r0          # r0 = tmp + s[tos]
    st r0, (r1)         # tmp = r0 
    halt



.pos 0x1000
tos: .long 15   # tos
tmp: .long 16   # tmp

.pos 0x2000
a:  .long 0     # a[0]
    .long 1     # a[1]
    .long 2     # a[2]

s:  .long 3     # s[0]
    .long 4     # s[1]
    .long 5     # s[2]
    .long 6     # s[3]
    .long 7     # s[4]