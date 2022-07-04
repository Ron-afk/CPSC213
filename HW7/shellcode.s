    ld $0x2f62696e, r0
    st r0, (r5)
    ld $0x2f736800, r1
    st r1, 4(r5)
    mov r5, r0
    ld $7, r1
    sys $2