.pos 0x100
                 ld $array, r0            # r0 = address of array[0]
                 ld $0x1, r1              # r1 = 1
                 ld $0x8, r2              # r2 = 8;
                 ld $first, r3            # r3 = address of first
                 ld (r0,r1,4), r4         # r4 = array[1]
                 st r4, 0x0(r3)           # first = array[1]
                 ld (r0,r2,4), r5         # r5 = array[8]
                 st r5, (r0,r1,4)         # array[1] = array[8]
                 ld (r3), r3              # r3 = first
                 st r3, (r0,r2,4)         # array[8] = first
                 halt                     # halt

.pos 0x1000
first:           .long 0xffffffff         # first
.pos 0x2000
array:           .long 0xffffffff         # array[0]
                 .long 0xffffffff         # array[1]
                 .long 0xffffffff         # array[2]
                 .long 0xffffffff         # array[3]
                 .long 0xffffffff         # array[4]
                 .long 0xffffffff         # array[5]
                 .long 0xffffffff         # array[6]
                 .long 0xffffffff         # array[7]
                 .long 0xffffffff         # array[8]
                 .long 0xffffffff         # array[9]