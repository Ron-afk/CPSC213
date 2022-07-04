.pos 0x0
    ld   $0x1028, r5            # allocate stack position
    ld   $0xfffffff4, r0        # r0 = -12
    add  r0, r5                 # allocate stack space for function
    ld   $0x200, r0             # r0 = 0x200
    ld   0x0(r0), r0            # r0 = 0x200[0]
    st   r0, 0x0(r5)            # store local variable
    ld   $0x204, r0             # r0 = 0x204
    ld   0x0(r0), r0            # r0 = value in 0x204
    st   r0, 0x4(r5)            # store value in 0x204 into stack r5[1] = *0x204
    ld   $0x208, r0             # r0 = 0x208
    ld   0x0(r0), r0            # r0 = *0x208
    st   r0, 0x8(r5)            # store value in 0x208 to stack r5[2] = *0x208
    gpc  $6, r6                 # get ra
    j    0x300                  # jump to function in 0x300
    ld   $0x20c, r1             # r1 = 0x20c
    st   r0, 0x0(r1)            # store value in r1 into 0x20c
    halt
.pos 0x200
    .long 0x00000000            # 
    .long 0x00000000            #
    .long 0x00000000            #
    .long 0x00000000            #
.pos 0x300
    ld   0x0(r5), r0            # callee prolouge r0 = r5[0] = a0
    ld   0x4(r5), r1            # callee prolouge r1 = r5[1] = a1
    ld   0x8(r5), r2            # callee prolouge r2 = r5[2] = a2
    ld   $0xfffffff6, r3        # r3 = -10
    add  r3, r0                 # r0 -= 10 = a0 -10
    mov  r0, r3                 # r3 = r0 = a0 - 10
    not  r3                     # ~r3
    inc  r3                     # r3 = 10 - a0
    bgt  r3, L6                 # if a0 < 10 goto L6 (default)
    mov  r0, r3                 # r3 = a0 - 10
    ld   $0xfffffff8, r4        # r4 = -8
    add  r4, r3                 # r3 = r3 -8 = a0 - 18
    bgt  r3, L6                 # if a0 > 18 goto L6 (default)
    ld   $0x400, r3             # r3 = 0x400
    ld   (r3, r0, 4), r3        # r3 = r3[a0-10] (jumptable)
    j    (r3)                   # goto r3
.pos 0x330
    add  r1, r2                 # r2 += r1 = a1 + a2 (case a0 = 10)
    br   L7                     # goto L7
    not  r2                     # ~r2
    inc  r2                     # r2 = -a2
    add  r1, r2                 # r2 = a1-a2
    br   L7                     # goto L7
    not  r2                     # ~r2
    inc  r2                     # r2 = -a2
    add  r1, r2                 #  r2 += r1 = a1 - a2
    bgt  r2, L0                 # if a1 > a2 goto L0
    ld   $0x0, r2               # r2 = 0
    br   L1                     # goto L1
L0:
    ld   $0x1, r2               # r2 = 1
L1:
    br   L7                     # goto L7
    not  r1                     # ~r1
    inc  r1                     # r1 = -a1
    add  r2, r1                 # r1 = a2 - a1 
    bgt  r1, L2                 # if a2 > a1 goto L2
    ld   $0x0, r2               # r2 = 0
    br   L3                     # goto L3
L2:
    ld   $0x1, r2               # r2 = 1
L3:
    br   L7                     # goto L7
    not  r2                     #  ~r2
    inc  r2                     # r2 = -a2
    add  r1, r2                 # r2 = a1 - a2
    beq  r2, L4                 # if a1 == a2 goto L4
    ld   $0x0, r2               # r2 = 0
    br   L5                     # goto L5
L4:
    ld   $0x1, r2               # r2 = 1
L5:
    br   L7                     # goto L7
L6:
    ld   $0x0, r2               # r2 =0
    br   L7                     # goto L7
L7:
    mov  r2, r0                 # r0 = r2
    j    0x0(r6)                # return

.pos 0x400                      # jumptable
    .long 0x00000330            # case a0 = 10
    .long 0x00000384            # L6 (case a0 = 11) defult
    .long 0x00000334            # case a0 = 12 
    .long 0x00000384            # case a0 = 13
    .long 0x0000033c            # case a0 = 14
    .long 0x00000384            # case a0 = 15
    .long 0x00000354            # case a0 = 16
    .long 0x00000384            # case a0 = 17
    .long 0x0000036c            # case a0 = 18
.pos 0x1000                     # stack
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000