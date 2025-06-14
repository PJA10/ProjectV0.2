; file matrix_error.as
; This file contains errors in .matrix directives and matrix operands.

; 1. this line is ok
M1:     .mat [2][2]  1,2,3,4 

; 2. this line is ok 
STR1:   .string "abcdefg"

; 3. this line is ok (not mandatory to provide a label)
     .mat [2][2]  1,2,3,4 

; 4. invalid register (r32)
       mov M1[r32][r3],r3 

; 5. invalid register (6)
       mov M1[6][r3],r3 
				
; 6. invalid matrix addressing
        mov M1[r1][r3][r3],r3 
		
; 7. invalid label name r2
        mov r2[r3][r3],r3 

; 8. invalid matrix addressing - space not allowed
        mov M1 [r3][r3],r3

; 9.  invalid matrix addressing
        mov M1[r3][r3,r3  
		
; 10. missing arguments in directive 
        .mat 

; 11. missing columm size
         .mat [2]

; 12. invalid value ("a")
        .mat [2][2]  "a", 2, 3, 4

; 13. invalid values (a)
        .mat [2][2]  a, 2, 3, 4
				
; 14. this line is ok
MAIN:            mov     M1[r2][r7],W 

; 15. this line is ok
        mov M1[r3][r3],r3 

; 16. this line is ok (spaces are allowed between commas)
M2:     .mat [2][2]  1, 2 , 3, 4 

; 17. this line is ok (not mandatory to provide values)
        .mat [2][2] 

; 17. invalid matrix row size
        .mat [-2][2] 