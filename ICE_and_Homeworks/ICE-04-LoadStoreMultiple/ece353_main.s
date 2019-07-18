; Filename:     main.s 
; Author:       ece353 staff 
; Description:  

    export __main

;******************************************
; Symbolic Constants
;****************************************** 
WORD    	EQU     4
HALF_WORD 	EQU 	2
BYTE 		EQU 	1
  
  
;******************************************
; SRAM
;******************************************
    AREA    SRAM, READWRITE
;******************************************
; Load Store Review
;******************************************


;******************************************
; Load Store Multiple
;******************************************
DEST_ARRAY     SPACE   32*WORD
DEST2_ARRAY    SPACE   32*WORD
DEST3_ARRAY    SPACE   32*WORD
RESULT		   SPACE   1*WORD
    align
        
        
        
        
        
;**********************************************
; FLASH
;**********************************************
    AREA    FLASH, CODE, READONLY
;******************************************
; Load Store Review
;******************************************


;******************************************
; Load Store Multiple
;******************************************
SRC_ARRAY   DCD         0x00000000
            DCD         0x00001111
            DCD         0x00002222
            DCD         0x00003333
            DCD         0x00004444
            DCD         0x00005555
            DCD         0x00006666
            DCD         0x00007777
            DCD         0x00008888 
            DCD         0x00009999
            DCD         0x0000AAAA
            DCD         0x0000BBBB
            DCD         0x0000CCCC
            DCD         0x0000DDDD
            DCD         0x0000EEEE
            DCD         0x0000FFFF
            DCD         0x11110000
            DCD         0x11111111
            DCD         0x11112222
            DCD         0x11113333
            DCD         0x11114444
            DCD         0x11115555
            DCD         0x11116666
            DCD         0x11117777
            DCD         0x11118888
            DCD         0x11119999
            DCD         0x1111AAAA
            DCD         0x1111BBBB
            DCD         0x1111CCCC
            DCD         0x1111DDDD
            DCD         0x1111EEEE
            DCD         0x1111FFFF
    

ASCII_ARRAY DCB			"ECE353"
			DCB			 0; terminate string

align



;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
__main   PROC
  
    ;******************************************
    ; Load Store Review
    ;******************************************
	
	; load the address of ASCII_ARRAY into R0
	ADR R0, ASCII_ARRAY  
	
	; load the address of result into R1
	LDR R1, =(RESULT)
	
	MOV R3, #0x0			; initialize sum
LOOP
		LDRB R2, [R0], #1	; first , load the byte in
		CMP R2, #0x0 		; check if R2 is 0x0
		BEQ ENDLOOP			; if it is, exit loop
		SUB R2, R2, #0x30	; subtract 30 from hex value for the ASCII character
		CMP R2, #0x9		; if (R2-30) is in the range 0-9, N flag will be set
		ADDLE R3, R3, R2	; if condition met, add to sum 
		B LOOP
ENDLOOP
	
	STR R3, [R1]			; store sum into RESULT 
    
    
    ;******************************************
    ; Load Store Multiple
    ;******************************************
    ; Load the address of SRC_ARRAY into R0
    ADR R0, SRC_ARRAY

    
    ; Load the address of DEST_ARRAY into R1
    LDR R1, =(DEST_ARRAY)

    
    ; Load the address of DEST2_ARRAY into R2
    LDR R2, =(DEST2_ARRAY)

       
    ; Using LDM, load the first 8 WORDs in SRC_ARRAY
    ; Use Registers R3-R10 as the destination 
    ; of the LDM instruction
    LDM R0, {R3-R10}

    
    ; Use STM to store R3-R10 to DEST_ARRAY
    STM R1, {R3-R10}

    
     ; Use STM to store R3-R10 to DEST2_ARRAY
     ; Specify the order of the registers in
     ; revers order (STM R2, {R10, R9, R8, R7, R6, R5, R4, R3})
     ; Observe if the order of the registers changes
     ; how the data arranged in SRAM
     STM R2, {R10, R9, R8, R7, R6, R5, R4, R3}


    
    ; Using LDM, load the second 8 WORDs in SRC_ARRAY
    ; Use Registers R3-R10 as the destination 
    ; of the LDM instruction.
    ; Hint, you will need to modify R0 so that
    ; it contains the address of SRC_ARRAY[8]

    ADD R0, R0, #32
    LDM R0, {R3-R10}


    ; Use STM to store R3-R10 to DEST_ARRAY[8]
    ; Hint, you will need to modify R1 so that
    ; it contains the address of DEST_ARRAY[8]

    ADD R1, R1, #32
    STM R1, {R3-R10}


    ; Copy the contents of SRC_ARRAY to DEST2_ARRAY
    ; 8 WORDs at a time using LDM/STM. This time use
    ; the version of LDM/STM that updates the base address
    ; Load the address of SRC_ARRAY into R0

    ; R0 <- ADDR[SRC_ARRAY[0]]
    SUB R0, R0, #32

    ;#############################
    ; load first 8 words
    ; R0 <- R0 + 32
    LDM R0!, {R3-R10}

    ; store first 8 words into DEST2_ARRAY
    ; R2 <- R2 + 32
    STM R2!, {R3-R10} 

    ;#############################
    ; load second 8 words
    ; R0 <- R0 + 32
    LDM R0!, {R3-R10}

    ; store second 8 words into DEST2_ARRAY
    ; R2 <- R2 + 32
    STM R2!, {R3-R10}

    ;#############################
    ; load third 8 words
    ; R0 <- R0 + 32
    LDM R0!, {R3-R10}

    ; store third 8 words into DEST2_ARRAY
    ; R2 <- R2 + 32
    STM R2!, {R3-R10}

    ;#############################
    ; load fourth 8 words
    ; R0 <- R0 + 32
    LDM R0!, {R3-R10}

    ; store fourth 8 words into DEST2_ARRAY
    ; R2 <- R2 + 32
    STM R2!, {R3-R10}



    ; Write code that uses a loop to copy SRC_ARRAY
    ; to DEST3_ARRAY.  Each iteration of the loop should
    ; copy 4 WORDs of the array
 
    ; Load the address of SRC_ARRAY into R0
    ADR R0, SRC_ARRAY

    ; Load the address of DEST3_ARRAY into R11
    LDR R11, =(DEST3_ARRAY)
	
	; initialize loop counter 
	MOV R12, #0x0 
	
FOR_START
		CMP R12, #0x8
		BEQ FOR_END
		LDM R0!, {R3-R6}
		STM R11!, {R3-R6}
		ADD R12, R12, #0x1
		B FOR_START
FOR_END
		
    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!
        
INFINITE_LOOP
    B INFINITE_LOOP
    
    ENDP
    align
        

    END            
