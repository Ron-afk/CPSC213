.pos 0x100
            ld  $4, r0
            ld  $a, r1
            ld  (r1, r0, 4), r2
            inc r0
            st  r2, (r1, r0, 4)
            halt
.pos 0x1000
a:          .long 17
            .long 2
            .long 0
            .long 13
            .long 7