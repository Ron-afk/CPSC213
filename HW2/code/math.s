.pos 0x100
    ld $o, r0         # r0 = address of o
    ld $w, r1         # r1 = address of w
    ld (r1), r2       # r2 = w
    ld (r1), r3       # r3 = w
    inc r3            # r3 = w+1
    inca r3           # r3 = w +1+4
    shl $0x1, r3      # r3 = (w +1 +4) <<1
    and r2, r3        # r3 = ((w +1 +4) <<1)  & w
    shr $0x2, r3    # r3= (((w +1 +4) <<1)  & w)/4
    st r3, (r0)     # o = (((w +1 +4) <<1)  & w)/4
    halt            # halt

.pos 0x1000
o: .long 0xffffffff # o

.pos 0x2000
w: .long 0xffffffff # w