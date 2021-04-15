section .data
extern _keyboard_interrupt
extern _reserved_interrupt
extern _hd_interrupt

global _keyboard_interrupt_entry
global _reserved_interrupt_entry0
global _reserved_interrupt_entry1
global _reserved_interrupt_entry0
global _reserved_interrupt_entry1
global _reserved_interrupt_entry2
global _reserved_interrupt_entry3
global _reserved_interrupt_entry4
global _reserved_interrupt_entry5
global _reserved_interrupt_entry6
global _reserved_interrupt_entry7
global _reserved_interrupt_entry8
global _reserved_interrupt_entry9
global _reserved_interrupt_entry10
global _reserved_interrupt_entry11
global _reserved_interrupt_entry12
global _reserved_interrupt_entry13
global _reserved_interrupt_entry14
global _reserved_interrupt_entry15
global _reserved_interrupt_entry16
global _reserved_interrupt_entry17
global _reserved_interrupt_entry18
global _reserved_interrupt_entry19
global _reserved_interrupt_entry20
global _reserved_interrupt_entry21
global _reserved_interrupt_entry22
global _reserved_interrupt_entry23
global _reserved_interrupt_entry24
global _reserved_interrupt_entry25
global _reserved_interrupt_entry26
global _reserved_interrupt_entry27
global _reserved_interrupt_entry28
global _reserved_interrupt_entry29
global _reserved_interrupt_entry30
global _reserved_interrupt_entry31
global _reserved_interrupt_entry32
global _reserved_interrupt_entry33
global _reserved_interrupt_entry34
global _reserved_interrupt_entry35
global _reserved_interrupt_entry36
global _reserved_interrupt_entry37
global _reserved_interrupt_entry38
global _reserved_interrupt_entry39
global _reserved_interrupt_entry40
global _reserved_interrupt_entry41
global _reserved_interrupt_entry42
global _reserved_interrupt_entry43
global _reserved_interrupt_entry44
global _reserved_interrupt_entry45
global _reserved_interrupt_entry46
global _reserved_interrupt_entry47
global _reserved_interrupt_entry48
global _reserved_interrupt_entry49

global _hd_interrupt_entry

section .text

_keyboard_interrupt_entry:
	push _keyboard_interrupt
	push 0x21
	;中断结束命令
	mov al,0x20
	out 0xa0,al
	out 0x20,al
no_error_code:
	xchg [esp+4],eax
	xchg [esp],ebx
	;保存上下文
	push ds
	push es
	push fs
	push gs
	pushad
	;内核代码数据段选择符
	mov edx,10h
	mov ds,dx
	mov es,dx
	mov fs,dx
	;真正调用中断处理函数
	push ebx
	call eax
	pop ebx
	;中断退出
	popad
	pop gs
	pop fs
	pop es
	pop ds
	pop ebx
	pop eax
	iretd

_hd_interrupt_entry:
	mov eax,[_hd_interrupt]
	push eax
	push 0x2e
	mov al, 0x20
	out 0x20, al
	out 0xA0, al
	jmp no_error_code

%macro _reserved_interrupt_entry 1
_reserved_interrupt_entry%1:
	push _reserved_interrupt
	push %1
	jmp no_error_code
%endmacro 

_reserved_interrupt_entry 0
_reserved_interrupt_entry 1
_reserved_interrupt_entry 2
_reserved_interrupt_entry 3
_reserved_interrupt_entry 4
_reserved_interrupt_entry 5
_reserved_interrupt_entry 6
_reserved_interrupt_entry 7
_reserved_interrupt_entry 8
_reserved_interrupt_entry 9
_reserved_interrupt_entry 10
_reserved_interrupt_entry 11
_reserved_interrupt_entry 12
_reserved_interrupt_entry 13
_reserved_interrupt_entry 14
_reserved_interrupt_entry 15
_reserved_interrupt_entry 16
_reserved_interrupt_entry 17
_reserved_interrupt_entry 18
_reserved_interrupt_entry 19
_reserved_interrupt_entry 20
_reserved_interrupt_entry 21
_reserved_interrupt_entry 22
_reserved_interrupt_entry 23
_reserved_interrupt_entry 24
_reserved_interrupt_entry 25
_reserved_interrupt_entry 26
_reserved_interrupt_entry 27
_reserved_interrupt_entry 28
_reserved_interrupt_entry 29
_reserved_interrupt_entry 30
_reserved_interrupt_entry 31
_reserved_interrupt_entry 32
_reserved_interrupt_entry 33
_reserved_interrupt_entry 34
_reserved_interrupt_entry 35
_reserved_interrupt_entry 36
_reserved_interrupt_entry 37
_reserved_interrupt_entry 38
_reserved_interrupt_entry 39
_reserved_interrupt_entry 40
_reserved_interrupt_entry 41
_reserved_interrupt_entry 42
_reserved_interrupt_entry 43
_reserved_interrupt_entry 44
_reserved_interrupt_entry 45
_reserved_interrupt_entry 46
_reserved_interrupt_entry 47
_reserved_interrupt_entry 48
_reserved_interrupt_entry 49

error_code:

	xchg [esp+4],eax

	;保存上下文
	push ds
	push es
	push fs
	push gs
	pushad

	;内核代码数据段选择符
	mov edx,10h
	mov ds,dx
	mov es,dx
	mov fs,dx

	;真正调用中断处理函数
	call eax

	;中断退出
	popad
	pop gs
	pop fs
	pop es
	pop ds
	pop eax

	iretd