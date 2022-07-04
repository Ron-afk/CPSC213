.pos 0x1000
code:
    ld $s, r0       # r0 = &s
    ld $v0, r1      # r1 =  &v0
    ld $v1, r2      # r2 = &v1
    ld $v2, r3      # r3 = &v2
    ld $v3, r4      # r4 = &v3
    ld $i, r5       # r5 = &i
    ld (r5), r5     # r5 = i
    ld (r0,r5,4),r6 # r6 = s.x[i]
    st r6, (r1)     # v0 = s.x[i]
    ld 8(r0),r6     # r6 = s.y
    ld (r6,r5,4),r7 # r7 = s.y[i]
    st r7, (r2)       # v1 = s.y[i]
    ld 12(r0),r6    # r6 = s.z
    ld (r6,r5,4),r7 # r7 = s.z->x[i]
    st r7, (r3)       # v2 = s.z->x[i]
    ld 12(r6), r6   # r6 = s.z.z
    ld 8(r6), r6    # r6 = s.z.z.y
    ld (r6,r5,4),r7 # r7 = s.z.z->y[i]
    st r7, (r4)       # v3 = s.z.z->y[i] 
    halt

.pos 0x2000
static:
i:  .long 0
v0: .long 0 
v1: .long 0
v2: .long 0
v3: .long 0
s:  .long 0     # x[0]
    .long 0     # x[1]
    .long s_y   # *y
    .long s_z     # *z

.pos 0x3000
heap:
s_y:    .long 0         # s.y[0]
        .long 0         # s.y[1]
s_z:    .long 0         # s.z -> x[0]
        .long 0         # s.z -> x[1]
        .long 0         # s.z -> y
        .long s_z_z     # s.z -> z
s_z_z:  .long 0         # s.z -> z -> x[0]
        .long 0         # s.z -> z -> x[1]
        .long s_z_z_y   # s.z -> z -> y
        .long 0         # s.z -> z -> z
s_z_z_y:.long 0         # s.z -> z -> y[0]
        .long 0         # s.z -> z -> y [1]