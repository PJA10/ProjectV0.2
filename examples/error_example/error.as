; This file is intended to check the bad-path of the assembler.

; 1. this line is ok (DATA4 is declared later)
Start:  dec DATA4

; 2. missing operand
        sub #5

; 3. missing operand
        sub #5,

; 4. missing operand
        red

; 5. this line is ok
       .extern DATA4

; 6. invalid target operand (immediate)
        add  #5,#6

; 7. this line is ok (immediate target operand allowed for cmp)
Next:   cmp  #5,#6

; 8. invalid operand (immediate)
        inc  #50

; 9. undefined instruction
        and  r1,r2

; 10. undefined instruction (case sensitivity)
        jSr  Start

; 11. this line is ok (r9 is a label declared later)
        add  r1,r9

; 12. invalid characters (,r2)
        cmp  r1,,r2

; 13. invalid characters (,r3)
        add  #5,r1,r3

; 14. invalid characters (blabla)
        prn r1 blabla

; 15. invalid characters (123)
        inc  123

; 16. invalid characters (xyz)
        cmp  #xyz,r1

; 17. label previously declared
Next:   clr  r2

; 18. label does not start in first column (optional error! label is ignored)
     label1:   sub r1,r7

; 19. invalid source operand (register)   
        lea  r3,r1

; 20. invalid source operand (immediate)   
        lea  #3,r5

; 21. invalid characters (Start)
        stop Start

; 22. invalid characters (400) 
        .data   200 400

; 23. this line is ok (arbitrary whitespace between operands)
DATA1:  .data   100,    	200 ,  -500, 300   		   

; 24. invalid characters (,3)
        .data   1, ,3

; 25. invalid character (,)
        .data   4,

; 26. invalid characters (#123)
        .data   #123

; 27. invalid characters (.4)
        .data   12.4

; 28. invalid characters (-5)
        .data   --5

; 29. this line is ok (case sensitive labels)
Data1:  .data   100, +200, -300

; 30. invalid label (cannot be an instruction)
mov:    .data   5

; 31. invalid label (cannot be a register)
r1:     .data   200,300

; 32. label previously declared
DATA1:  .data   300

; 33. invalid label(non-alphabetic first character)
1DATA:  .data   300

; 34. this line is ok (r9 is not the name of a register)
r9:     .data   200

; 35. this line is ok (label X declaration ignored - warning may be issued)
X:      .entry  DATA1

; 36. this line is ok (it is ok to declare the same external more than once)
        .extern DATA4

; 37. local label cannot be declared as external
        .extern Start    
		
; 38. this line is ok (label Y declaration ignored - warning may be issued)
Y:      .extern DATA8

; 39. this line is ok (label STOP is declared later)
        .entry  STOP

; 40. label is already designated as external
        .entry  DATA4

; 41. undefined instruction (note: DATA2 is not a label declaration)
DATA2   .data   4

; 42. undefined directive (case sensitivity)
        .DATA   5

; 43. This line is ok (it is ok to designate the same entry more than once)
        .entry  DATA1

; 44. invalid characters (blabla is not a string)
        .string blabla

; 45. invalid characters (blabla)
        .string "abcdefg" blabla

; 46. this line is ok (comma within string is not a separator)
STR1:   .string "abc, ,defg"

; 47. invalid label (too long)
SuperCalifragilisticExpiAlidocious: .data	4	
          
; 48. missing argument in directive
        .data

; 49. missing argument in directive
        .entry

; 50. undefined directive
        .invalid 85,90

; 51. this line is ok
        rts

; 52. invalid characters (Next)
        rts Next

; 53. this line is ok
STOP:   stop
