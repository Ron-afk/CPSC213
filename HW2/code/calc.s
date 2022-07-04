.pos 0x100
    ld $s, r0  # r0 = address of s
    ld $j, r1  # r1 = address of j
    ld $i, r2  # r2 = address or i
    ld $t, r3  # r3 = address of t[0]
    ld (r0), r4  # r4 = s
    ld (r0), r5  # r5 = s
    ld $0x6, r6  # r6 = 6
    add r6, r5  # r5 = s + 6
    ld (r3, r5, 4), r6  # r6 = t[s]
    ld (r3, r4, 4), r7  # r7 = t[s+6]
    add r6, r7   # r7 = t[s] + t[s+6]
    st r7, (r2)  # i = r7 = t[s] + t[s+6]
    ld $0xf0f0f0f, r6   # r6 = $0xf0f0f0f
    and r6, r7  # r7 = i & 0xf0f0f0f
    st r7, (r1)  # j = r7 = i & 0xf0f0f0f
    halt     # halt


.pos 0x1000
s: .long 0xffffffff   # s
j: .long 0xffffffff   # j
i: .long 0xffffffff   # i

.pos 0x2000
t: .long 0xffffffff  # t[0]
   .long 0xffffffff  # t[1]
   .long 0xffffffff  # t[2]
   .long 0xffffffff  # t[3]
   .long 0xffffffff  # t[4]
   .long 0xffffffff  # t[5]
   .long 0xffffffff  # t[6]
   .long 0xffffffff  # t[7]
   .long 0xffffffff  # t[8]

