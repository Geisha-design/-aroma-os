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

/* ��addr����ʼ��word_cnt����д��˿�port */
static inline void outsw(short port, const void* addr, int word_cnt) {
	/*********************************************************
	   +��ʾ�����Ƽ��������������.
	   outsw�ǰ�ds:esi����16λ������д��port�˿�, ���������ö�������ʱ,
	   �Ѿ���ds,es,ss�ε�ѡ���Ӷ�����Ϊ��ͬ��ֵ��,��ʱ���õ������ݴ��ҡ�*/
	asm volatile ("cld; rep outsw" : "+S" (addr), "+c" (word_cnt) : "d" (port));
	/******************************************************/
}

/* ���Ӷ˿�port�����word_cnt����д��addr */
static inline void insw(short port, void* addr, int word_cnt) {
	/******************************************************
	   insw�ǽ��Ӷ˿�port�������16λ����д��es:ediָ����ڴ�,
	   ���������ö�������ʱ, �Ѿ���ds,es,ss�ε�ѡ���Ӷ�����Ϊ��ͬ��ֵ��,
	   ��ʱ���õ������ݴ��ҡ�*/
	asm volatile ("cld; rep insw" : "+D" (addr), "+c" (word_cnt) : "d" (port) : "memory");
	/******************************************************/
}
