#define outb(value,port) \
	__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
	__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
	_v; \
})

#define outb_p(value,port) \
	__asm__ ("outb %%al,%%dx\n" \
			"\tjmp 1f\n" \
			"1:\tjmp 1f\n" \
			"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
	__asm__ volatile ("inb %%dx,%%al\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:":"=a" (_v):"d" (port)); \
	_v; \
})

/* 将addr处起始的word_cnt个字写入端口port */
static inline void outsw(short port, const void* addr, int word_cnt) {
	/*********************************************************
	   +表示此限制即做输入又做输出.
	   outsw是把ds:esi处的16位的内容写入port端口, 我们在设置段描述符时,
	   已经将ds,es,ss段的选择子都设置为相同的值了,此时不用担心数据错乱。*/
	asm volatile ("cld; rep outsw" : "+S" (addr), "+c" (word_cnt) : "d" (port));
	/******************************************************/
}

/* 将从端口port读入的word_cnt个字写入addr */
static inline void insw(short port, void* addr, int word_cnt) {
	/******************************************************
	   insw是将从端口port处读入的16位内容写入es:edi指向的内存,
	   我们在设置段描述符时, 已经将ds,es,ss段的选择子都设置为相同的值了,
	   此时不用担心数据错乱。*/
	asm volatile ("cld; rep insw" : "+D" (addr), "+c" (word_cnt) : "d" (port) : "memory");
	/******************************************************/
}
