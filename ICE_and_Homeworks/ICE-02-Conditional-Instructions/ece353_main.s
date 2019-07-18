; Filename:     main.s 
; Author:       ece353 staff 
; Description:  

    export __main

;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
    align

;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
__main   PROC
    
    ; Initialization of R0 and R1
	AND R0, #0xFFFF0000
    MOV     R0, #15
    MOV     R1, #100
    
    ; Using only conditional instructions (no branches)
    ; Implement the if statemens below.
	
	; Update the ASPR register
	; Z = 1, N = 0, C = 0, V = 0
    CMP R0, #15
    
    
    ; (1) If R0 == 15, then R2 = 100 , else R2 = 1
	MOVEQ R2, #100
	MOVNE R2, #1

    ; (2) If R0 != 15, then R3 = 200 , else R3 = 2
	MOVNE R3, #200
	MOVEQ R3, #2
	
    
    ; (3) If R0 < 15, then  R4 = 300 , else R4 = 3
	MOVLT R4, #300
	MOVGE R4, #3

    ; (4) If R0 <= 15, then R5 = 400 , else R5 = 4
	MOVLE R5, #400
	MOVGT R5, #4
    
    ; (5) If R0 > 15,  then R6 =500 , else R6 = 5
	MOVGT R6, #500
	MOVLE R6, #5
    
    ; (6) If R0 >= 15, then R7 = 600 , else R7 = 6
	MOVGE R7, #600
	MOVLT R7, #6
    
 
    MOV     R0, #0x2
    ; (7) Implement the following switch statement using only conditional instructions
    
    ; Hint: Start by setting R1 to the default value and then checking all of the
    ; other cases.
    
	; Z = 1, N = 0, C = 0, V = 0
	
	
	
	; default case
	MOV32 R1, #0xFFFFFFFF
	
	; if R0 = 0
	CMP R0, #0x0
	MOVEQ R1, #0x0
	
	; if R0 = 1
	CMP R0, #0x1
	MOVEQ R1, #10
	
	; if R0 = 2
	CMP R0, #0x2
	MOVEQ R1, #20
	
	; if R0 = 3
	CMP R0, #0x3
	MOVEQ R1, #30
	
	
    ; switch(R0)
    ; {
    ;      case 0:
    ;      {
    ;           R1 = 0;
    ;           break;
    ;      }
    ;      case 1:
    ;      {
    ;           R1 = 10;
    ;           break;
    ;      }
    ;      case 2:
    ;      {
    ;           R1 = 20;
    ;           break;
    ;      }
    ;      case 3:
    ;      {
    ;           R1 = 30;
    ;           break;
    ;      }
    ;      default:
    ;      {
    ;           R1 = 0xFFFFFFFF;
    ;           break;
    ;      }
    ;  }
    


    ; Initialization of R0 and R1
    MOV     R0, #15
    MOV     R1, #100
    
	; If A and B do this, else do that:
	; Do this
	; Do that if NOT A
	; Do that if NOT B
	
	; If A or B do this, else do that:
	; Do that
	; Do this if  A
	; Do this if  B
	
    ; (8) If R0 >= 15 OR R1 < 150, then R8 = 700 , else R8 = 7
	
	; Do the else statement first
	MOV R8, #7
	; Do the if statement if the if statement IS true
	CMP R0, #15
	MOVGE R8, #700
	
	CMP R1, #150
	MOVLT R8, #700
    
    
    ; (9) If R0 >= 15 AND R1 < 150, then R9 = 800, else R9 = 8
	
	; Do the if statement first
	MOV R9, #800
	; do the else statements by checking if the if statement is NOT true
	CMP R0, #15
	MOVLT R9, #8
	
	CMP R1, #150
	MOVGE R9, #8
	

    MOV32   R0, #0xFFF000FF
    ; (10) If bit 2 OR bit 21 of register R0 is a 1, then R1 = 900 , else R1 = 9
	
	; Do the else statement first
	MOV R1, #9
	; Do the if statement if the if statement IS true
	TST R0, #0x4
	MOV R1, #900
	
	TST R0, #0x200000
	MOV R1, #900
 
    
    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!	
        
INFINITE_LOOP
    B INFINITE_LOOP
    
    ENDP
    align
        

    END            
