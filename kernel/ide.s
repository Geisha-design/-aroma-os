global _read_disk

;----��Ӳ�̷�����eaxΪlba�����ţ�ebxΪ��д���ڴ��ַ��ecxΪ�����������
_read_disk:

	pushad
	
	mov eax,[esp+8]
	mov ebx,[esp+4]
	mov ecx,[esp]

	mov esi,eax	;����
	mov edi,ecx	;����

;��һ��������Ҫ��ȡ��������
	mov dx,0x1f2
	mov al,cl
	out dx,al
	mov eax,esi	;�ָ�
	
;�ڶ���������LBA��ַ
	mov cl,8
	;0-7λд��0x1f3
	mov dx,0x1f3
	out dx,al
	;8-15λд��0x1f4
	mov dx,0x1f4
	shr eax,cl
	out dx,al
	;16-23λд��0x1f5
	mov dx,0x1f5
	shr eax,cl
	out dx,al
	;24-27λд��0x1f6
	mov dx,0x1f6
	shr eax,cl
	and al,0x0f	;lba��24-27λ
	or al,0xe0	;����4λΪ1110����ʾlbaģʽ
	out dx,al
	
;��������д�������
	mov dx,0x1f7
	mov al,0x20
	out dx,al

;���Ĳ������Ӳ��״̬
.not_ready:
	nop
	in al,dx
	and al,0x88	;��4λΪ1��ʾ׼���ã���7λΪ1��ʾæ
	cmp al,0x08
	jnz .not_ready
	
;���岽��������
	mov eax,edi
	mov dx,256
	mul dx
	mov ecx,eax ;�ָ�
	
	mov dx,0x1f0
	.go_on_read:
		in ax,dx
		mov [ebx],ax
		add ebx,2
		loop .go_on_read

popad
ret