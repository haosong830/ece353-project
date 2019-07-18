; hw1.s ECE353


    export hw1_search_memory
	import WS2812B_write

WORD        EQU     4
HALF_WORD   EQU     2
BYTE        EQU     1

;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
LED_ARRAY	SPACE 	6*WORD
    align
		
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
	align
; All functions are callee saved.


; Description
;	Updates the WS2812B LEDs with the current values in the LED_ARRAY
;	Uses the supplied function WS2812B_write to update the colors

; Parameters
;	NONE
 
; Returns
;	NOTHING
;
hw1_update_leds PROC
	; Need to save LR for nested function call
	PUSH {R1, R2, LR}
	; Load Array address and # LEDS for function call to WS2812B_write
	LDR R1, =(LED_ARRAY)
	MOV R2, #8
	BL WS2812B_write
	POP {R1, R2, PC}
	BX LR
    ENDP
		
		

; Description
 ;	Converts a single ASCII HEX character to its numerical value. 
 ;	Valid characters are 0-9, a-f , A-F

 ; Parameters
 ;	R0 - ASCII Char

 ; Returns
 ;	R0 - numerical value. If invalid, return 0xFFFFFFFF
;
hw1_ascii_to_hex PROC
	; Need to save LR for nested function call
	PUSH {R1, LR}
	
	; 'a'-'f' = 0x61 - 0x66
	; 'A'-'F' = 0x41 - 0x46
	; '0'-'9' = 0x30 - 0x39
characters_a_f
	; First check if greater than 'f'
	CMP R0, #0x66
	; If it is, invalid, so set R0 to -1 and return
	MOVGT R0, #0xFFFFFFFF
	BGT return
	; Then check if less than 'a'
	CMP R0, #0x61
	; if it is, then check if 'A' through 'F'
	BLT characters_A_F
	; otherwise, character is 'a' - 'f'. 
	; Subtract #87 to convert to hex value and return! :) 
	SUB R0, R0, #87
	B return
	
characters_A_F
	; Now check if greater than 'F'
	CMP R0, #0x46
	; If it is, invalid, so set R0 to -1 and return
	MOVGT R0, #0xFFFFFFFF
	BGT return
	; Then check if less than 'A'
	CMP R0, #0x41
	; if it is, finally check if '0' - '9'
	BLT characters_0_9
	; otherwise, character is 'A' - 'F'. 
	; subtract #55 to convert to hex value and return!  :)
	SUB R0, R0, #55
	B return

characters_0_9
	; if not 'a'-'f' or 'A'-'F' call function to see if '0'-'9'
	; if not '0'-'9' either, R0 will be 0xFFFFFFFF
	BL hw1_ascii_to_dec
return
	POP {R1, PC}
	BX LR
    ENDP


; Description
 ;	Converts a single ASCII DECIMAL character to its numerical value. 
 ;	Valid characters are 0-9. 
 ;  Will be called from within hw1_ascii_to_hex

 ; Parameters
 ;	R0 - ASCII Char
 
 ; Returns
 ;  R0 -  numerical value. If invalid, return 0xFFFFFFFF
;
hw1_ascii_to_dec PROC
	; '0'-'9' = 48 - 57 (decimal)
	; Check if less than '0'. And update flags
	SUBS R0, #48
	; if it is, invalid
	MOVLT R0, #0xFFFFFFFF
	BX LR
    ENDP


; Description
 ;	Turns off all of the WS2812B LEDs by writing 0x00000000 to each LED
 ;	Should be called with 'LOAD'

 ; Parameters
 ;	NONE
 
 ; Returns
 ;	NOTHING
;
hw1_init PROC
	PUSH {R0-R2, LR}
	; set R0 to 0
	MOV R0, #0x00000000
	; load array address 
	LDR R1, =(LED_ARRAY)
	
	
	; LED_ARRAY is 24 bytes, or 6 words. 
	; Set all of those bytes to zero.
	STR R0, [R1], #4
	STR R0, [R1], #4
	STR R0, [R1], #4
	STR R0, [R1], #4
	STR R0, [R1], #4
	STR R0, [R1]
	
	; call write function to
	; actually change colors of LEDs
	LDR R1, =(LED_ARRAY)
	MOV R2, #8
	BL WS2812B_write

	POP {R0-R2, PC}
	BX LR
    ENDP

; Description
 ;   Updates the color of the specified LED. All other LEDs should maintain
 ;	 their current color. Makes use of the hw1_update_leds to update
 ;   the color of the LEDs.
    
 ; Parameters
 ;   R0 - LED # to be updated. Assume 0 is the leftmost LED
 ;   R1 - An unsigned 32 bit #. Bits 31-24 are unused. Bits 23-0 
 ;	 represent the color value to write to the LED

 ; Returns
 ;   NOTHING
;
hw1_ledx PROC
	PUSH {R2-R7, LR}
	
	; Each LED takes up 3 bytes to store the color
	; R3 = green
	; R4 = red
	; R5 = blue
	
	; Isolate the color value bits using shifts
	
	; The blue value is stored in R1[7:0]
	MOV R5, R1, LSL #24
	MOV R5, R5, LSR #24
	; The red value is stored in R1[15:8]
	MOV R4, R1, LSL #16
	MOV R4, R4, LSR #24
	; The green value is stored in R1[23:16]
	MOV R3, R1, LSL #8
	MOV R3, R3, LSR #24
	
	; load the address of the led array into R2
	LDR R2, =(LED_ARRAY)
	
	; calculate the correct led # address
	MOV R7, #7
	; R6 <-- (7 - LED#)
	SUB R6, R7, R0
	; R6 <-- (7-LED#) * 3
	MOV R7, #3
	MUL R6, R6, R7
	; LED_ARRAY + correct offset 
	ADD R2, R2, R6
	
	; store green byte in LED_ARRAY + offset + 0
	STRB R3, [R2, #0]
	; store red byte in LED_ARRAY + offset + 1
	STRB R4, [R2, #1]
	; store blue byte in LED_ARRAY + offset + 2
	STRB R5, [R2, #2]
	
	; With LED_ARRAY updated, call the update
	; leds function to do the updating
	BL hw1_update_leds
	
	POP{R2-R7, PC}
	BX LR
    ENDP
	
		
; Description
 ;	Delays the examination of the next memory address by a variable 
 ;	amount of time (hang)
 ; Parameters
 ;	R0 - A 32 bit unsigned number representing the # of iterations
 ;	of an empty for loop that must be executed.

 ; Returns
 ;	NOTHING
;
hw1_wait PROC
	PUSH {R0}
	; Use the # iterations as a counter
	; To make the first iteration count, must add 1.
	; because first thing loop does is subtract one
	ADD R0, R0, #1
hang_loop_start
	SUBS R0, #1
	; If (#iterations - 1) <= 0, then end loop
	BLS hang_loop_end
	; Otherwise, keep looping
	B hang_loop_start
hang_loop_end
	POP {R0}
	BX LR
    ENDP


; Description
 ;	This function will search through memory a byte at a time looking for valid DISPLAY
 ;	commands. When a valid DISPLAY command is found, carry out the corresponding
 ;	behavior described in the DISPLAY command table above. Memory that holds invalid
 ;	commands are ignored. This function will examine memory until and HALT command
 ;	is found.

 ; Parameters
 ;	R0 - The starting address of the memory that contains the commands
 ;	     to update LEDs.

 ; Returns
 ;	NOTHING
;
hw1_search_memory PROC
	
	; Command 			Bytes		Description
	;LOAD				4			Turn all LEDs off.

	;HALT				4			Leave LEDs in current state, enter infinite loop

	;HANG				8			HANGXXXX = empty loop that executes X,XXX * 10,000	
	;								times and then exits. XXXX is a 4 digit ASCII string

	;LEDx				10			x represents which LED is being modified
	;								6 remaining characters indicate color intensity. 
	;								5 & 6 = Green
	;								7 & 8 = Red
	;								9 & 10 = Blue
	;								Example for 2nd LED : LED2FF8800
	
	PUSH{R0-R4, LR}
	
	; R0 is used in ascii_to_hex and ascii_to_dec 
	; so must save in another register
	MOV R2, R0

; Check if command is 'LOAD' which would get loaded into
; R4 as 'DAOL' since ARM is little endian. Compare the loaded word to
; the hex represenation of 0xDAOL
start_search

	; check if command is LOAD
	MOV32 R3, #0x44414F4C
	LDR R4, [R2]
	CMP R4, R3
	BNE command_LEDx
	
	; if all command was 'LOAD'
	; then set all bytes
	; in LED_ARRAY to 0
	BL hw1_init
	; increment the DISPLAY_CMD address in R2
	; and check next command
	ADD R2, R2, #1
	B start_search
	
; Check if command is 'LEDx' which would get loaded into
; R4 as 'xDEL' since ARM is little endian. Compare the loaded
; word from R2 to hex representation of '0DEL'
command_LEDx
	
	LDR R4, [R2]
	; clear out the x in LEDx so can see if command is LED
	BIC R5, R4, #0xFF000000
	; hex represenation of 0DEL
	MOV32 R3, #0x0044454C
	CMP R5, R3
	; if not the right command, check if HANG command
	BNE command_HANG
	
	
	; Once we know it's LEDx, then get value of 'x'
	; by shifting the value of the x byte to the LSB of R0
	LSR R0, R4, #24
	
	; numerical value is returned in R0
	BL hw1_ascii_to_hex
	; first check if valid
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; if valid, check if 0-7
	CMP R0, #7
	; if invalid, then increment command address and 
	; start search again
	ADDGT R2, R2, #1
	BGT start_search
	
	; save value of R0 in R4 so we can use it for call
	; to ledx later
	MOV R4, R0
	
	; now that we know its LEDx, check the next 6 bytes after 'x' to 
	; get the color values and load them into R1
	; hw1_ledx takes R0 as the #LED and R1 as the color values
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;GREEN;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #4] ; R0 <-- character 5 (green)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting green color 
	MOV R1, R0
	
	LDRB R0, [R2, #5] ; R0 <-- character 6 (green)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting green color 
	LSL R1, R1, #4
	ORR R1, R1, R0
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;RED;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #6] ; R0 <-- character 7 (red)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting red color 
	LSL R1, R1, #4
	ORR R1, R1, R0
	
	LDRB R0, [R2, #7] ; R0 <-- character 8 (red)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting red color 
	LSL R1, R1, #4
	ORR R1, R1, R0
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;BLUE;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #8] ; R0 <-- character 9 (blue)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting blue color 
	LSL R1, R1, #4
	ORR R1, R1, R0
	
	LDRB R0, [R2, #9] ; R0 <-- character 10 (blue)
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; valid result, so now put the resulting red color 
	LSL R1, R1, #4
	ORR R1, R1, R0
	
	; now the green value is in R1[23:16]
	; the red in R1[15:8]
	; and blue in R1[7:0]
	
	; call ledx function to set the colors
	; R1 holds the colors
	; R0 holds the LED. Get LED# from R4 where we saved previously
	MOV R0, R4
	BL hw1_ledx
	
	; increment to next command
	ADD R2, R2, #1
	; now, start search over and check all possibilties
	; of the next command
	B start_search

; To check if 'HANG', see if loaded word in R4 is equivalent
; to 'GNAH', since ARM is little endian. Do this by checking if the hex
; represenation of 'GNAH' is equal to loaded word
command_HANG
	LDR R4, [R2]
	MOV32 R3, #0x474E4148
	CMP R4, R3
	; if loaded word and hex representation of 'TLAH' not equal
	; then check if command is 'HALT'
	BNE command_HALT
	
	; now that we know command is HANG, must check next
	; 4 bytes of characters to see how long to halt for
	
	; example 4 digit ASCII value = 5604
	;;;;;;;;;;;;;;;;;;;;;;;;1000th place;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #4]
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; if valid, this number is in the 1000th place so
	; multiply it by 1000 and place in R3
	MOV R4, #1000
	; R3 <-- 5 * 1000
	MUL R3, R0, R4
	
	;;;;;;;;;;;;;;;;;;;;;;;;100th place;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #5]
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; if valid, this number is in the 100th place so
	; multiply it by 100 and add to sum of what to multiply in R3
	MOV R4, #100
	; R3 <-- 5*1000 + (6*100)
	MLA R3, R4, R0, R3
	
	;;;;;;;;;;;;;;;;;;;;;;;;10th place;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #6]
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; if valid, this number is in the 10th place so
	; multiply it by 10 and add to sum of what to multiply in R3
	MOV R4, #10
	; R3 <-- 5*1000 + (6*100) + (0*10)
	MLA R3, R4, R0, R3
	
	;;;;;;;;;;;;;;;;;;;;;;;;1s place;;;;;;;;;;;;;;;;;;;;;;;;
	LDRB R0, [R2, #7]
	BL hw1_ascii_to_hex
	; first check if valid result
	CMP R0, #0xFFFFFFFF
	; if invalid, then increment command address and 
	; start search again
	ADDEQ R2, R2, #1
	BEQ start_search
	; if valid, this number is in the 1s place so
	; add to sum of what to multiply and place
	; in R0 because that is what hw1_wait takes in
	; R0 <-- 5*1000 + (6*100) + (0*10) + (4*1)
	ADD R0, R3, R0
	
	; now R3 has the 4 digit value of how much to halt for
	; though, before calling hw1_wait, multiply R3 by 10,000
	MOV R4, #10000
	MUL R0, R0, R4
	
	; now wait for specified amount of time by calling
	; wait function that takes R0 as a parameter
	BL hw1_wait
	
	; increment the DISPLAY_CMD address in R2
	; and check next command
	ADD R2, R2, #1
	B start_search
	
; To check if 'HALT', see if loaded word in R4 is equivalent
; to 'TLAH', since ARM is little endian. Do this by checking if the hex
; represenation of 'TLAH' is equal to loaded word
command_HALT
	
	LDR R4, [R2]
	MOV32 R3, #0x544C4148
	CMP R4, R3
	; if loaded word and hex representation of 'TLAH' not equal
	; then increment the address of the command array and start search again
	ADDNE R2, R2, #1
	BNE start_search
infinite_HALT_loop
	; If command is 'HALT' leave LEDs
	; in current state and enter an infinite loop.
	B infinite_HALT_loop
	
	
	; restore changed registers
	POP{R0-R4, PC}
	; return
	BX LR
    ENDP
	
    align        
    
    END



