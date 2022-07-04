.pos 0x100
main:
        ld $s, r2       # r2 = &s r2 stores the current student 
        ld (r2),r2      # r2 = s
        ld $n, r0       # r0 = &n
        ld (r0), r0     # r0 = n
        gpc $6, r6      # recorde return pos
        j inter_avg     # calculate students average grade
        gpc $6, r6
        j sort_iter     # sort student based on their average grade
        gpc $6, r6
        j median        
        halt

        
inter_avg:
        beq r0, back_main    # if r0 == 0, all student have their average, goto main
        j s_avg_grade   # calculate next student average grade
        
next_avg:
        dec r0          # n--
        ld $24, r7      # r7 = 24
        add r7, r2      # r2 = next student
        br inter_avg    # next iteration

s_avg_grade:    
        ld $0, r3       # r3 = 0
        ld 4(r2),r4     # r4 = s.grade[0]
        add r4, r3      # r3 = sum s.grade
        ld 8(r2),r4     # r4 = s.grade[1]
        add r4, r3      # r3 = sum s.grade
        ld 12(r2),r4    # r4 = s.grade[2]
        add r4, r3      # r3 = sum s.grade
        ld 16(r2),r4    # r4 = s.grade[3]
        add r4, r3      # r3 = sum s.grade
        shr $2, r3      # r3 = avg grade
        st r3, 20(r2)   # student average grade = r3
        br next_avg     # return


sort_iter:
        ld $n, r0       # r0 = &n
        ld (r0), r0     # r0 = temp_n  
        ld $s, r2       # r2 = &s r2 stores the current student 
        ld (r2),r2      # r2 = s
out_loop:
        dec r0          # r0 = temp_i
        beq r0, back_main    # if r0 == 0, outer loop over, back to main
        ld $1, r1       # r1 = 1 = temp_j
in_loop:
        mov r0, r7      # r7 = r0 = temp_i
        not r7          
        inc r7          # r7 = -temp_i
        add r1, r7      # r7 = temp_j - temp_i
        bgt r7, out_loop    # if temp_j > temp_i goto outloop
        mov r1, r4      # r4 = r1
        mov r1, r5      # r5 = r1
        shl $4, r4      # r4 = 16 * r1
        shl $3, r5      # r5 = 16 * r1
        add r5, r4      # r4 = 24 * r1
        add r4, r2      # r2 = address of first student to compare and move
        j compare_swap  # jump to compare and swap
end_compare:
        ld $s, r2       # r2 = &s r2 stores the current student 
        ld (r2),r2      # r2 = s
        inc r1          # temp_j++
        br in_loop      # jump back to inner loop for next iteration



compare_swap: 
        # r2 is the student to compare and swap
        ld 20(r2), r4     # r4 = average of current student
        ld $-4, r7
        mov r2, r3
        add r7,r3       
        ld (r3), r5     # r5 = student average of previous student
        mov r4, r7
        not r7
        inc r7          # r7 = - average grade of the second student 
        add r5, r7      # r7 = previouse student average - student average
        bgt r7, swap    # if average of previous student is larger, swap
        j end_compare      # jump back to in_loop



swap: 
        # r2 is the first student to swap, r3 is the second student to swap
        ld (r2), r4     # swap sid
        ld $-24, r7
        mov r2, r3
        add r7,r3  
        ld (r3), r5
        st r4, (r3)
        st r5, (r2)

        ld 4(r2), r4     # swap grade[0]
        ld 4(r3), r5
        st r4, 4(r3)
        st r5, 4(r2)

        ld 8(r2), r4     # swap grade[1]
        ld 8(r3), r5
        st r4, 8(r3)
        st r5, 8(r2)

        ld 12(r2), r4     # swap grade[2]
        ld 12(r3), r5
        st r4, 12(r3)
        st r5, 12(r2)

        ld 16(r2), r4     # swap grade[3]
        ld 16(r3), r5
        st r4, 16(r3)
        st r5, 16(r2)

        ld 20(r2), r4     # swap avg
        ld 20(r3), r5
        st r4, 20(r3)
        st r5, 20(r2)

        j end_compare        # return


median:
        ld $n, r0           # r0 = &n
        ld (r0), r0         # r0 = n
        ld $s, r2           # r2 = &s r2 stores the current student 
        ld (r2),r2          # r2 = s
        shr $1, r0          # r0 = n / 2
        mov r0, r1          # r1 = r0
        mov r0, r3          # r3 = r0
        shl $4, r1          # r1 = r0 * 16
        shl $3, r3          # r3 = r0 * 8
        add r1, r3          # r3 = 24 * r0
        add r3, r2          # r2 = address of the student at median
        ld (r2), r4         # r4 = sid of median student
        ld $m, r5           # r5 = &m
        st r4, (r5)         # m = sid of median
        j back_main         # retrun

        
        
back_main:  
        j (r6)              # back main


.pos 0x1000
n:      .long 2       # just one student
m:      .long 0       # put the answer here
s:      .long base    # address of the array
base:   .long 1234    # student ID
        .long 77      # grade 0
        .long 60      # grade 1
        .long 50      # grade 2
        .long 90      # grade 3
        .long 0       # computed average        
s_2:   .long 1235    # student ID
        .long 40      # grade 0
        .long 60      # grade 1
        .long 78      # grade 2
        .long 90      # grade 3
        .long 0       # computed average 
s_3:   .long 1236    # student ID
        .long 80      # grade 0
        .long 99      # grade 1
        .long 78      # grade 2
        .long 90      # grade 3
        .long 0       # computed average      

s_4:   .long 1237    # student ID
        .long 80      # grade 0
        .long 100      # grade 1
        .long 78      # grade 2
        .long 50      # grade 3
        .long 0       # computed average    

s_5:   .long 1238    # student ID
        .long 77      # grade 0
        .long 92      # grade 1
        .long 78      # grade 2
        .long 78      # grade 3
        .long 0       # computed average    
  