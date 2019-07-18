	export bubble_sort
	export swap_values

	
	AREA    FLASH, CODE, READONLY
    ALIGN
   
	
;******************************************************************************
; Description
;     Given the address in R7, it will read the unsigned byte at R7 and R7 + 1.
;     If [R7] > [R7 + 1], swap the values
;
;     Modify only registers R8 or greater.
;
; Parameters
;   R7 - Array Address
;
; Returns
;   Nothing
;******************************************************************************
swap_values PROC
     ;---------------------------------------
     ; START ADD CODE
     ;---------------------------------------
	 
	 ; read bytes at [R7] and [R7 + 1]
	 LDRB R8, [R7]							
	 LDRB R9, [R7, #1]
	 ; see if byte at [R7] > byte at [R7 + 1]
	 CMP R8, R9
	 
	 ; store contents of [R7] into [R7 + 1]
	 STRBGT R8, [R7, #1]
	 ; store contents of [R7 + 1] into [R7]
	 STRBGT R9, [R7]
	 
	 BX LR
	 
     ;---------------------------------------
     ; END ADD CODE
     ;---------------------------------------
    ENDP


    
;******************************************************************************
; Description
;   Uses Bubble Sort to sort an array of unsigned 8-bit numbers.
;
;   Modify only registers R0-R7
;
; Parameters
;   R0 - Array Address
;   R1 - Array Size
;
; Returns
;   Nothing
;******************************************************************************
bubble_sort PROC

    ; Save registers
     PUSH   {R0-R12, LR}
     
     ;---------------------------------------
     ; START ADD CODE
     ;---------------------------------------
	 
	 SUB R1, R1, #1 ; size = size - 1
	
	
outer_while_start
	 CMP R1, #0     ; R1 - 0
	 BEQ outer_while_end
	 MOV R2, #0	   ; index = 0
	 
inner_while_start
	 CMP R2, R1					; while index < size
	 BHS inner_while_end		; if index >= then branch
	 
	 ADD R7, R0, R2    			; R7 <-- Base Address + Index. R7 is what swap_values uses
	 BL swap_values ; swap values
	 ADD R2, R2, #1  ; index++
	 B inner_while_start
	
inner_while_end
	 SUB R1, R1, #1 ; size--
	 B outer_while_start
	 
outer_while_end
		
     ; NOTE: The return from the function is already
     ; provided below
     
     ;---------------------------------------
     ; END ADD CODE
     ;---------------------------------------
     
    
    ; Restore Registers
    POP     {R0-R12, LR}
    
    ; Return from the function
    BX      LR
    
    ENDP
         
    END     

