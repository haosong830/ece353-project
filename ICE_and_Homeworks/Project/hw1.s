; hw1.s ECE353
BYTE	EQU		1
WORD	EQU		4
NUM_LEDS EQU	8
LED_ARRAY_SIZE	EQU	24*BYTE
    export hw1_search_memory
	import WS2812B_write
		
LOAD_FOUND	RN	R5 ; define R5 to contain the info whether LOAD is found
HALT_FOUND	RN	R6 ; define R6 to contain the info whether HALT is found
HANG_FOUND	RN	R7 ; define R7 to contain the info whether HANG is found
LEDx_FOUND 	RN  R8 ; define R8 to contain the info whether LEDx is found

;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
LED_ARRAY	SPACE	LED_ARRAY_SIZE
    align
		
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
hw1_update_leds PROC	
	
	PUSH	{R1, R2, LR} ;SAVE modified registers onto stack
	LDR R1, =(LED_ARRAY) ;fetch starting address of LED_Array, load it into R1*
	MOV R2, #NUM_LEDS ; LOAD R2 with number of leds
	BL	WS2812B_write ; branch to the WS2812B_write function
	POP		{R1, R2, LR} ; pop saved registers back
	BX	LR	
	
	ENDP
		
hw1_ascii_to_hex PROC
	
	CMP	R0, #0x30 ;compare the value in R0 with 0x30, to see if it is than ASCII char of 0
	MOVLT R0, #0xFFFFFFFF ;if it is less than 0x30, then move invalid into R0
	BLT END_FUNC ; end the function if the character is invalid
	
	CMP	R0, #0x66 ; compare R0 to 0x66
	MOVGT R0, #0xFFFFFFFF ; if greater than 0x66, it's out of bounds.
	BGT END_FUNC ; end the function 
	
	CMP R0, #0x39 ; compare R0 with 0x39, if <=, that means the character falls in valid bound(0-9)
	SUBLE R0, R0, #48 ; subtract 48 from R0 to convert character to its hex value
	BLE END_FUNC ; end function
	
	CMP R0, #0x41 ; compare R0 with 0x41, if <= that means it falls into invalid bound
	MOVLT R0, #0xFFFFFFFF ; move invalid into R0
	BLT END_FUNC; end function
	
	CMP R0, #0x46 ; compare R0 with 0x46, if <=, that means R1 falls into valid bound(A-F)
	SUBLE R0, R0, #55 ; subtract 55 from R0 to get its hex value
	BLE END_FUNC ; end function
	
	CMP R0, #0x61 ; compare R0 with 0x61, if <= that means it falls into invalid bound
	MOVLT R0, #0xFFFFFFFF ; move invalid into R0
	BLT END_FUNC ; end function
	
	SUB R0, R0, #87 ; the only possiblity left is the valid bound(a-f), we convert R0 to hex by subtracting 87
	
END_FUNC
	BX LR ; end function, branch to the caller
	
	ENDP
		
hw1_ascii_to_dec PROC

	CMP	R0, #0x30 ;compare the value in R0 with 0x30, to see if it is less than ASCII char of 0
	MOVLT R0, #0xFFFFFFFF ;if it is, then move invalid into R0
	BXLT LR ; end the function if the character is invalid
	
	CMP R0, #0x39 ; compare R0 with 0x39, if <=, that means the character falls in valid bound(0-9)
	SUBLE R0, R0, #48 ; subtract 48 from R0 to convert character to its dec value
	BXLE LR ; end function
	
	MOV R0, #0xFFFFFFFF ; move invalid into R0 if no conditions met(not in 0x30-0x39)
	BX LR
	
	ENDP
		
hw1_init	PROC
	
	PUSH	{R0,R1,R2} ; we are using r0-r2, so save them to stack first
	MOV R2, #0 ; initialize R2 to 0 to write it back to led_array
	MOV R1, #LED_ARRAY_SIZE ; initialize R1 to the size of the array
	LDR	R0,	=(LED_ARRAY) ; let R0 contain the starting address of the array
LOOP
	CMP	R1, #0 ; if R1 <= 0, then we have initialized all 24 bytes in the array, we can exit function.
	BLE END_LOOP ; branch to the end of loop if R1 <= 0
	STRB R2, [R0], #1 ; store 0 back to the array, and increment the starting address by 1 to access next byte
	SUB R1, R1, #1 ; we've done 1 iteration, so subtract 1 from R1(24)
	B	LOOP ; start looping through the array again
END_LOOP
	POP {R0-R2} ; restore values in R0-R2
	BX	LR ; branch to the caller
	
	ENDP

hw1_ledx	PROC
	
	PUSH	{R1, R2, LR} ; fitst save all registers we are modifying to the stack, including the LR because we have nested function call
	LDR	R2, =(LED_ARRAY) ; load the starting address of the LED_ARRAY into R2
	;logic: compare R0(the number of LED to be write) with 0-7. If R0 equal to any number between 0 and 7, we then store the first byte into the
	;blue slot(little endian) by adding offset, then shift right by 8 bits to move the RED byte to the right, store it into the right slot by adding corresponding
	;offset. At last shift right R1 again by 8 bits to push the GREEN byte to the right, store it to the GREEN slot, and we are done. After modifying
	;the LED_ARRAY, we call the hw1_update_leds function to write the color value into each LED GPIO port.
	CMP	R0, #7
	STRBEQ R1, [R2, #2] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #1]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2]
	
	CMP	R0, #6
	STRBEQ R1, [R2, #5] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #4]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #3]
	
	CMP	R0, #5
	STRBEQ R1, [R2, #8] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #7]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #6]
	
	CMP	R0, #4
	STRBEQ R1, [R2, #11] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #10]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #9]
	
	CMP	R0, #3
	STRBEQ R1, [R2, #14] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #13]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #12]
	
	CMP	R0, #2
	STRBEQ R1, [R2, #17] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #16]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #15]
	
	CMP	R0, #1
	STRBEQ R1, [R2, #20] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #19]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #18]
	
	CMP	R0, #0
	STRBEQ R1, [R2, #23] 
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #22]
	LSREQ R1, R1, #8
	STRBEQ R1, [R2, #21]

	BL	hw1_update_leds
	POP	{R1, R2, PC}
	
	ENDP
	
hw1_wait PROC
		
	PUSH {R0, R1} ; save registers we are modifying
	MOV	R1, #10000 ; we will multiply value in R0 by 10000, so load 10000 into a register first
	MUL R0, R0, R1
	;start to wait: we compare if R0 has reached 0 or not. If yes, we end the loop. If not, we then subtract R0 by 1, and continue waiting(loop again)
LOOP_WAIT
	CMP	R0, #0
	BLE END_WAIT
	SUB R0, R0, #1
	B LOOP_WAIT
END_WAIT
	;pop used registers, branch back to caller
	POP {R0, R1}
	BX	LR
	
	ENDP
		
;******************************************************************************** 
; This function is used to find whether a word is command LOAD. It will return 1
; to R5(LOAD_FOUND) if the command is indeed LOAD. Else it returns 0 to R5
; Parameters
; R0 - The starting address of the command string 

; Returns:
; 1 for a found LOAD. 0 for LOAD not found.
;********************************************************************************      
hw1_seeIfLOAD	PROC
	
	PUSH {R1} ; save R1 because we are going to use it
	
	;compare 1st byte
	LDRB R1, [R0]; load the first byte of command into R1, and add the pointer to points to next byte
	CMP R1, #76 ; compare the char in R1 with 'L'
	LDRBEQ R1, [R0, #1]! ;if R1 == 'L', load next byte, increment pointer by 1
	BNE NOT_LOAD ; if R1 != 'L', end searching
	
	;compare 2nd byte
	CMP R1, #79 ; compare R1 with 'O'
	LDRBEQ R1, [R0, #1]! ;if R1 == 'O', load next byte, increment pointer by 1
	SUBNE R0, R0, #1 ; we subtract the address by an offset is to go back to the last char and check for other cases. Same logic applies
	;to every subtraction below
	BNE NOT_LOAD ; if R1 != 'O', end searching
	
	;compare 3rd byte
	CMP R1, #65 ; compare R1 with 'A'
	LDRBEQ R1, [R0, #1]! ; ;if R1 == 'A', load next byte, increment pointer by 1
	SUBNE R0, R0, #2
	BNE NOT_LOAD ; if R1 != 'A', end searching
	
	;compare 4th byte
	CMP R1, #68 ; compare R1 with 'D'
	ADDEQ R0, R0, #1 ; ;if R1 == 'D', then LOAD is found, increment the pointer by 1
	SUBNE R0, R0, #3
	BNE NOT_LOAD ; if R1 != 'D', end searching

	MOV LOAD_FOUND, #1 ; we found a LOAD command, assert the LOAD_FOUND
	POP {R1}
	BX LR

NOT_LOAD
	MOV LOAD_FOUND, #0 ; we didn't found a LOAD command, assert the LOAD_FOUND
	POP {R1}
	BX LR
	
	ENDP

		
;******************************************************************************** 
; This function is used to find whether a word is command HALT. It will return 1
; to R6(HALT_FOUND) if the command is indeed HALT. Else it returns 0 to R6
; Parameters
; R0 - The starting address of the command string 

; Returns:
; 1 for a found HALT. 0 for HALT not found.
;********************************************************************************     
hw1_seeIfHALT	PROC
		
	PUSH {R1} ; save R1 because we are going to use it
	
	;compare 1st byte
	LDRB R1, [R0]; load the first byte of command into R1, and add the pointer to points to next byte
	CMP R1, #72 ; compare the char in R1 with 'H'
	LDRBEQ R1, [R0, #1]! ;if R1 == 'H', load next byte, increment pointer by 1
	BNE NOT_HALT ; if R1 != 'H', end searching
	
	;compare 2nd byte
	CMP R1, #65 ; compare R1 with 'A'
	LDRBEQ R1, [R0, #1]! ;if R1 == 'A', load next byte, increment pointer by 1
	SUBNE R0, R0, #1
	BNE NOT_HALT ; if R1 != 'A', end searching
	
	;compare 3rd byte
	CMP R1, #76 ; compare R1 with 'L'
	LDRBEQ R1, [R0, #1]! ; ;if R1 == 'L', load next byte, increment pointer by 1
	SUBNE R0, R0, #2
	BNE NOT_HALT ; if R1 != 'L', end searching
	
	;compare 4th byte
	CMP R1, #84 ; compare R1 with 'T'
	ADDEQ R0, R0, #1 ; ;if R1 == 'T', then HALT is found, increment the pointer by 1
	SUBNE R0, R0, #3
	BNE NOT_HALT ; if R1 != 'T', end searching

	MOV HALT_FOUND, #1 ; we found HALT command, assert the HALT_FOUND
	POP {R1}
	BX LR
	
NOT_HALT
	MOV HALT_FOUND, #0 ; This word is not HALT, the search for HALT fails
	POP {R1}
	BX LR
	ENDP
;******************************************************************************* 
; This function is used to find whether a word is command HANG. It will return 1
; to R7(HANG_FOUND) if the command is indeed HANG. Else it returns 0 to R7
; Parameters
; R0 - The starting address of the command string 

; Returns:
; 1 for a found HANG. 0 for HANG not found.
;********************************************************************************    	
hw1_seeIfHANG	PROC
	
	PUSH {R1, R2, LR} ; we are going to use R1 and R2 so save it first
	MOV R2, R0 ;reserve the address in R0 using R2
	
	;compare 1st byte
	LDRB R1, [R2]; load the first byte of command into R1, and add the pointer to points to next byte
	CMP R1, #72 ; compare the char in R1 with 'H'
	BNE FAIL_HANG ; if R1 != 'H', end searching
	LDRBEQ R1, [R2, #1]! ;if R1 == 'H', load next byte, increment pointer by 1
	
	;compare 2nd byte
	CMP R1, #65 ; compare R1 with 'A'
	SUBNE R2, R2, #1
	BNE FAIL_HANG ; if R1 != 'A', end searching
	LDRBEQ R1, [R2, #1]! ;if R1 == 'A', load next byte, increment pointer by 1
	
	;compare 3rd byte
	CMP R1, #78 ; compare R1 with 'N'
	SUBNE R2, R2, #2
	BNE FAIL_HANG ; if R1 != 'N', end searching
	LDRBEQ R1, [R2, #1]! ; ;if R1 == 'N', load next byte, increment pointer by 1
	
	;compare 4th byte
	CMP R1, #71 ; compare R1 with 'G'
	SUBNE R2, R2, #3
	BNE FAIL_HANG ; if R1 != 'G', end searching
	ADD R2, R2, #1 ; ;if R1 == 'G', then HANG is found, increment the pointer by 1
	
	;compare remaing 4 HEX numbers By calling hw1_ascii_to_hex up to 4 times. If we get FFFFFFFFF in R0, then the searching fails.
	LDRB R0, [R2]; load the first hex number at address stored in R2 to R0 because we are using R0 to pass parameter
	BL	hw1_ascii_to_dec ; will return a value into R0
	CMP R0, #0xFFFFFFFF
	BEQ FAIL_HANG
	MOV R10, #1000	
	MUL R1, R0, R10 ; To move the MSB to the 16th bit
	
	
	ADD R2, R2, #1 ; increment the pointer by 1 to access next byte
	LDRB R0, [R2]; load the second hex number at address stored in R2 to R0 because we are using R0 to pass parameter
	BL	hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	SUBEQ R2, R2, #1
	BEQ FAIL_HANG
	MOV R10, #100
	MUL R0, R0, R10; move the 2nd MSB to the 12th bit
	ADD R1, R1, R0
	
	ADD R2, R2, #1; increment the pointer by 1 to access next byte
	LDRB R0, [R2]; load the third hex number at address stored in R2 to R0 because we are using R0 to pass parameter
	BL	hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	SUBEQ R2, R2, #2
	MOV R10, #10
	MUL R0, R0, R10; move the 2nd MSB to the 12th bit 
	ADD R1, R1, R0
	
	ADD R2, R2, #1; increment the pointer by 1 to access next byte
	LDRB R0, [R2]; load the fourth hex number at address stored in R2 to R0 because we are using R0 to pass parameter
	BL	hw1_ascii_to_dec
	CMP R0, #0xFFFFFFFF
	SUBEQ R2, R2, #3
	BEQ FAIL_HANG
	ADD R1, R1, R0
	MOV R0, R1 ; we will pass the wait time as a parameter through R0, so we need to move the value in R1 back to R0
	
	BL hw1_wait ; call wait function here because we already have the wait_time stored in R0
	MOV HANG_FOUND, #1 ; if wait gets executed, that means HANG was found
	ADD R2, R2, #1 ; increment the pointer to point to next char
	MOV R0, R2 ; move the address in R2 back to R0 
	POP {R1, R2, PC} ; restore registers
	
FAIL_HANG
	MOV HANG_FOUND, #0 ; This word is not HANG, the search for HANG fails
	MOV R0, R2 ;restore the value in RO from R2
	POP {R1, R2, PC}
	
	ENDP
;******************************************************************************* 
; This function is used to find whether a word is command LEDx. It will return 1
; to R7(LEDx_FOUND) if the command is indeed HANG. Else it returns 0 to R8
; Parameters
; R0 - The starting address of the command string 

; Returns:
; 1 for a found HANG. 0 for HANG not found.
;********************************************************************************  
hw1_seeIfLEDx	PROC
	
	PUSH {R1, R2, R3, LR} ; we need to use R0 and R1 to store parameters passed to ledx function so we store their original values in to R2, R3

	MOV R2, R0
	
	LDRB R1, [R2] ; load the byte at the address stored in R2 intO r1
	;compare the 1st byte with 'L'
	CMP R1, #76 ; compare R1 with 'L'
	BNE FAIL ; if R1 != 'L', search fails
	LDRBEQ R1, [R2, #1]! ; ;if R1 == 'L', load next byte, increment pointer by 1
	
	CMP R1, #69 ; compare R1 with 'E'
	SUBNE R2, R2, #1
	BNE FAIL ; if R1 != 'L', search fails
	LDRBEQ R1, [R2, #1]! ; ;if R1 == 'E', load next byte, increment pointer by 1
	
	CMP R1, #68 ; compare R1 with 'D'
	SUBNE R2, R2, #2
	BNE FAIL ; if R1 != 'L', search fails
	ADD R2, R2, #1 ; let pointer point to next byte(should be a decimal character)
	
	LDRB R0, [R2] ; load the byte stored at address in R2 to R0(should be a decimal character)
	BL hw1_ascii_to_dec ; convert the byte in R0 to a dec numerical value(if success)
	CMP R0, #0xFFFFFFFF ; to see if conversion is failed
	BEQ FAIL ;  search for LEDx fails if FFFFFFFFF is returned in R0
	MOV R3, R0 ; save the returned value in R0 to R3
	
	;update GREEN COLOR
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #1
	BEQ FAIL
	LSL R1, R0, #20 ; left shift the MSB of the first GREEN color hex number to 23th bit, and store the result to R1
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #2
	BEQ FAIL
	ADD R1, R1, R0, LSL #16 ; left shift the MSB of the second GREEN color hex number to 19th bit, and ADD the result to R1
	
	;update RED COLOR
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #3
	BEQ FAIL
	ADD R1, R1, R0, LSL #12 ; left shift the MSB of the first RED color hex number to 15 bit, and ADD the result to R1
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #4
	BEQ FAIL
	ADD R1, R1, R0, LSL #8 ; left shift the MSB of the second RED color hex number to 11th bit, and ADD the result to R1
	
	;update BLUE COLOR
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #5
	BEQ FAIL
	ADD R1, R1, R0, LSL #4 ; left shift the MSB of the first BLUE color hex number to 15 bit, and ADD the result to R1
	ADD R2, R2, #1; if last byte is a valid dec character, then increment pointer by 1 to access next character
	LDRB R0, [R2]; load the byte stored at address in R2 to R0(should be a hex character)
	BL hw1_ascii_to_hex ; check whether the character falls into valid hex number bounds
	CMP R0, #0xFFFFFFFF ; if invalid is returned in R0, then search for LEDx fails
	SUBEQ R2, R2, #6
	BEQ FAIL
	ADD R1, R1, R0 ; ADD the second converted hex number of BLUE to R1 to get final COLOR intensity(24-bit unsigned number, with 31-24 bits set to 0)
	
	;update the specific LED by calling hw1_ledx
	MOV R0, R3; move the register number back to R0 as a parameter
	BL hw1_ledx
	ADD R2, R2, #1 ; increment the pointer to point to next char
	MOV R0, R2; restore the array address from R2 to R0
	MOV LEDx_FOUND, #1 ; indicates that we found a valid LEDx command
	POP {R1, R2, R3, PC}
	
FAIL
	MOV LEDx_FOUND, #0 ; indicates we found an invalid command
	MOV R0, R2; restore the array address from R2 to R0
	POP {R1, R2, R3, PC}
	
	ENDP
		
hw1_search_memory PROC
	
	PUSH {R1, LR} ; we are going to use R1 to store the or results of different FOUND registers
KEEP_SEARCHING ; if HALT is not found keep searching the memory
	BL	hw1_seeIfHALT
	CMP	HALT_FOUND, #1
	POPEQ {R1, PC}
	
	BL hw1_seeIfLOAD
	CMP LOAD_FOUND, #1
	BLEQ hw1_init
	
	BL hw1_seeIfHANG
	
	BL hw1_seeIfLEDx
	;the following 4 lines of code is to see whether the command is LOAD, HANG or LEDx. If none of them, then we increment the pointer by 1
	;and start to check next character
	ORR R1, LOAD_FOUND, HANG_FOUND
	ORR R1, R1, LEDx_FOUND
	CMP R1, #1
	ADDNE R0, R0, #1
	
	B KEEP_SEARCHING ; keep searching(not returning from the function) until we find a HALT
		
	ENDP
		
    align        
    
    END



