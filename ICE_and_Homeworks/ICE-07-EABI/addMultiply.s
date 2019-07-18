; Filename:     addMultiply.s 
; Author:       ece353 staff 
; Description:  

    export addMultiply

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
; Four arrays of 8-bit signed numbers are 
; passed via the first four parameters.
; The 5th paramter indicates the length of the
; arrays. For each entry in the array, the
; following operation takes place.
;
; Array3[i] = (Array0[i] + Array1[i]) * Array2[i]
;
; Parameter 0       Array Address 0
; Parameter 1       Array Address 1
; Parameter 2       Array Address 2
; Parameter 3       Array Address 3
; Parameter 4       Array Size 
;
; Returns
;   if ALL parameters are valid, return 0
;   else return -1.
;
;  An address is valid if it is non zero
;  The size is valid if it is greater than zero
;**********************************************
addMultiply PROC
    
	; R0 = Array Address 0
	; R1 = Array Address 1
	; R2 = Array Address 2
	; R3 = Array Address 3
	
	; Validate Address Parameters (nonzero)
	CMP R0, #0
	MOVEQ R0, #-1
	BEQ invalid_parameter
	
	CMP R1, #0
	MOVEQ R0, #-1
	BEQ invalid_parameter
	
	CMP R2, #0
	MOVEQ R0, #-1
	BEQ invalid_parameter
	
	CMP R3, #0
	MOVEQ R0, #-1
	BEQ invalid_parameter
	
	; Must save anything besides R0-R3. Caller must save R0-R3
	; R4 will hold the array size
	; R5 will hold Array0[i] and the sum of Array0 and Array1 and the product of the sum and Array2
	; R6 will hold Array1[i]
	; R7 will hold Array2[i]
	PUSH {R4-R7}
	
	; Get 5th parameter
	LDR R4, [SP, #0]
	; Validate Size Parameter (greater than zero)
	CMP R4, #0
	MOVLE R0, #-1
	BEQ array_loop_end
	
	
	
    ; For each index in the arrays, compute  
    ; Array3[i] = (Array0[i] + Array1[i]) * Array2[i]
array_loop_begin
	; compare array size to 0
	CMP R4, #0
	BEQ array_loop_end
	
	; since each entry is 8 bits, only have to offset 
	; by 1 byte each time. No shifting of i necessary
	; load array values. Signed values
	LDRSB R5, [R0], #1
	LDRSB R6, [R1], #1
	LDRSB R7, [R2], #1
	
	;(Array0[i] + Array1[i])
	ADD R5, R5, R6
	;(Array0[i] + Array1[i]) * Array2[i]
	MUL R5, R5, R7
	
	; store result in Array3[i]
	STR R5, [R3], #1
	
	; decrement size of array
	SUB R4, R4, #1
	; keep looping
	B array_loop_begin
array_loop_end
	; valid return value
	MOV R0, #0
	; if any parameters were invalid, R0 won't be set to 0
invalid_parameter

    ; Restore registers saved to the stack
	POP {R4-R8}
	
	; valid return value
	
    ; Return from the loop
	BX LR
	
	
    ENDP
    
    align
        

    END            
