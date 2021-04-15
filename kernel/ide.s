global _read_disk

;----读硬盘方法，eax为lba扇区号，ebx为待写入内存地址，ecx为读入的扇区数
_read_disk:

	pushad
	
	mov eax,[esp+8]
	mov ebx,[esp+4]
	mov ecx,[esp]

	mov esi,eax	;备份
	mov edi,ecx	;备份

;第一步，设置要读取的扇区数
	mov dx,0x1f2
	mov al,cl
	out dx,al
	mov eax,esi	;恢复
	
;第二步，设置LBA地址
	mov cl,8
	;0-7位写入0x1f3
	mov dx,0x1f3
	out dx,al
	;8-15位写入0x1f4
	mov dx,0x1f4
	shr eax,cl
	out dx,al
	;16-23位写入0x1f5
	mov dx,0x1f5
	shr eax,cl
	out dx,al
	;24-27位写入0x1f6
	mov dx,0x1f6
	shr eax,cl
	and al,0x0f	;lba的24-27位
	or al,0xe0	;另外4位为1110，表示lba模式
	out dx,al
	
;第三步，写入读命令
	mov dx,0x1f7
	mov al,0x20
	out dx,al

;第四步，检测硬盘状态
.not_ready:
	nop
	in al,dx
	and al,0x88	;第4位为1表示准备好，第7位为1表示忙
	cmp al,0x08
	jnz .not_ready
	
;第五步，读数据
	mov eax,edi
	mov dx,256
	mul dx
	mov ecx,eax ;恢复
	
	mov dx,0x1f0
	.go_on_read:
		in ax,dx
		mov [ebx],ax
		add ebx,2
		loop .go_on_read

popad
ret