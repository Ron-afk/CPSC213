.pos 0x100
        ld $n, r1           # r1 = &n
        ld (r1), r1         # r1 = n
        ld $a, r2           # r2 = &a
        ld $b, r3           # r3 = &b
        ld $c, r7           # r7 = &c
        ld (r7), r7         # r7 = c
        ld $0, r0           # temp_i = 0
        not r1
        inc r1              # r1 = -n
loop:   mov r0, r4          # r4 = r0        
        add r1, r4          # r4 = r1 - r0 = n - i
        beq r4, end         # if n=i goto end
        ld (r2,r0,4), r5    # r5 = a[i]
        ld (r3,r0,4), r6    # r6 = b[i]
        mov r6, r4          # r4 = b[i]
        not r4
        inc r4              # r4 = -b[i]
        add r5, r4          # r4 = a[i] - b[i]
        bgt r4, then        # if a[i]>b[i], gotot inc
        br end_if           # goto end_if

then:  inc r7               # r++
end_if: inc r0              # r0++ = i++
        br  loop            # goto loop
end:    ld $c, r1           # r0 = &c
        st r7, (r1)         # c = r7
        ld $i, r4           # r4 = &i
        st r0, (r4)         # i = temp_i
        halt 


.pos 0x1000
i:  .long -1
n:  .long 5
c:  .long 0
a:  .long 10
    .long 20
    .long 30
    .long 40
    .long 50
b:  .long 11
    .long 20
    .long 28
    .long 44
    .long 48
