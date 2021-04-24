.686P
.model flat,c
ExitProcess PROTO STDCALL :DWORD
includelib  kernel32.lib
includelib  libcmt.lib
includelib  legacy_stdio_definitions.lib
FLAG_TO_0 macro v1
	PUSH EAX
	MOV	EAX,0
	MOV v1,EAX
	POP	EAX
	endm
FLAG_TO_1 macro v1
	PUSH EAX
	MOV	EAX,1
	MOV	v1,EAX
	POP EAX
	endm
printf          PROTO C :VARARG
scanf           PROTO C :VARARG
clock           PROTO C :VARARG
EQULES_FUN PROTO C :VARARG
SOLD_FUN PROTO C :VARARG
FIND_FUN PROTO C :VARARG
SUPPLE_FUN PROTO C :VARARG
CLU_FUN PROTO C :VARARG
SORT_FUN PROTO C :VARARG
public lpFmt1,lpFmt2,lpFmt3,lpFmt4,lpFmt,FIND_ITEM,FIND_FAIL,SOLD_ITEM,NOT_ENOUGH,SUPPLE_ITEM
public SOLD_NUM,SUPPLE_NUM,MAX_NUM,MIN_NUM,IN_ITEM,I_NUM,TEMP,J_NUM,FLAG_SOLD,FLAG_EQULES,GA1,GA5
GOODS STRUCT
 GOODSNAME DB 10 DUP(0)
 BUYPRICE DW 0
 SELLPRICE DW 0
 BUYNUM DW 0
 SELLNUM DW 0
 RATE DW 0
GOODS ENDS
.DATA
lpFmt1	DB	"%s",0ah,0dh,0
lpFmt2	DB	"%s",0
lpFmt3	DB  "%d",0
lpFmt4	DB	"%hd",0ah,0dh,0
lpFmt   DB	"%d seconds",0ah, 0dh, 0
freq_l  dword  0
freq_h  dword  0
BNAME   DB	'TANXUAN',3 DUP(0) 
BPASS   DB	'U201915033',0
GA1	GOODS <'PEN',15, 20, 70, 25, ?>
GA2	GOODS <'PENCIL',2, 3, 100, 50, ?>
GA3	GOODS <'BOOK',30, 40, 25, 5, ?>
GA4	GOODS <'RULER',3, 4, 200, 150, ?>
GA5	GOODS <'MAX',0,0,0,0,255>
OP DD ?
SOLD_NUM	DW	?
SUPPLE_NUM	DW	?
USER_LOG	DB	'Please enter your username:',0
PASSWORD_LOG	DB	'Please enter your password:',0
LOGIN_FAIL	DB	'wrong username or password',0
TABLE_MENU	DB	'Please enter number1-5 to choose function'
					DB 0AH,0DH,'1:Find products'
					DB 0AH,0DH,'2:Shipment'
					DB 0AH,0DH,'3:replenishment'
					DB 0AH,0DH,'4:Calculate the profit margin of goods'
					DB 0AH,0DH,'5:Sort'
					DB 0AH,0DH,'6:Clock'
					DB 0AH,0DH,'7:Exit',0
CHOOSE_FAIL	DB	'wrong number',0
FIND_ITEM	DB	'Please enter the item name:',0
FIND_FAIL	DB	'Fail to find',0
SOLD_ITEM	DB	'Please input the quantity you want to sell',0
NOT_ENOUGH	DB	'It is not enough',0
SUPPLE_ITEM	DB	'Please input how many goods you want to replenish',0
EXIT_END	DB	'Sign out',0
IN_NAME		DB	10 DUP(0)
IN_PWD		DB	10 DUP(0)
IN_ITEM		DB	10 DUP(0)
I_NUM		DD	0
MAX_NUM		DW	0
MIN_NUM		DW	0
TEMP		DD	0
J_NUM		DD	0
FLAG_SOLD	DD	0
FLAG_EQULES DD	1
.STACK 200
.CODE
main proc c
LOGNAME:
	INVOKE printf,OFFSET lpFmt1 ,OFFSET USER_LOG
	INVOKE scanf,OFFSET lpFmt2 ,OFFSET IN_NAME
LOG1:
	LEA ESI,BNAME
	LEA EDI,IN_NAME
	MOV	EBX,0
	CALL EQULES_FUN
	CMP FLAG_EQULES,1
	JZ	LOGPWD
	INVOKE printf,OFFSET lpFmt1,OFFSET LOGIN_FAIL
	MOV EAX,1
	MOV FLAG_EQULES,EAX
	JMP LOGNAME
LOGPWD:
	INVOKE printf,OFFSET lpFmt1,OFFSET PASSWORD_LOG
	INVOKE scanf,OFFSET lpFmt2,OFFSET IN_PWD
LOG2:
	LEA ESI,BPASS
	LEA EDI,IN_PWD
	MOV	EBX,0
	CALL EQULES_FUN
	CMP FLAG_EQULES,1
	JZ  MENU
	INVOKE printf,OFFSET lpFmt1,OFFSET LOGIN_FAIL
	MOV EAX,1
	MOV FLAG_EQULES,EAX
	JMP LOGPWD
MENU:
	INVOKE printf,OFFSET lpFmt1,OFFSET TABLE_MENU
	INVOKE scanf,OFFSET lpFmt3,OFFSET OP
	CMP op,1
	JE FIND
 	CMP op,2
	JE SOLD
	CMP op,3
	JE SUPPLE
	CMP op,4
	JE CLU
	CMP op,7
	JE EXIT
	CMP op,5
	JE SORT
	CMP op,6
	JE	CLO
	INVOKE printf,OFFSET lpFmt1,OFFSET CHOOSE_FAIL
	JMP MENU
FIND:
	CALL FIND_FUN
	JMP MENU
SOLD:
	CALL SOLD_FUN
	JMP	MENU
SUPPLE:
	CALL SUPPLE_FUN
	JMP MENU
CLU:
	CALL CLU_FUN
	JMP MENU
SORT:
	CALL SORT_FUN
	JMP MENU
CLO:
	rdtsc
    sub eax, ecx
    sbb edx, ebx  
    mov freq_l, eax
    mov freq_h, edx 
    rdtsc         
	mov ecx, eax
	mov ebx, edx
	push ecx
	push ebx
    mov ecx,1000
L1:
    PUSH ecx
    call  CLU_FUN
    call  SORT_FUN
    pop ecx
    dec ecx 
    jnz L1

	rdtsc
	pop ebx
	pop ecx
	sub eax, ecx
	sbb edx, ebx  
	div freq_l
	invoke printf, offset lpFmt, eax
jmp  MENU
EXIT:
	INVOKE printf,OFFSET lpFmt1,OFFSET EXIT_END
	INVOKE ExitProcess,0
main endp
END
