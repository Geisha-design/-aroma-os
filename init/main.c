#include "debug.h"
#include "trap.h"
#include "keyboard.h"
#include "asm/system.h"
#include "asm/io.h"
#include <time.h>

extern void read_disk(int lba, int buff, int cnt);
extern void hd_init();
extern void hd_out(unsigned int count, unsigned int sect, unsigned int head, unsigned int cyl, unsigned int cmd);

// 为了证明确实执行到此处特意设置的无效值
static long count = 0;

static void* b;

extern void do_hd_read;
extern void do_hd_write;
extern int hd_interrupt;

extern long kernel_mktime(struct tm* tm);
long startup_time = 0;

#define PORT_DISK0_DATA				0x1f0
static char write_buf[256] = {1,};

#define CMOS_READ(addr) ({ \
outb_p(0x80|addr,0x70); \
inb_p(0x71); \
})

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

static void time_init(void) {
	struct tm time;

	do {
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8) - 1;
		time.tm_year = CMOS_READ(9);
	} while (time.tm_sec != CMOS_READ(0));
	BCD_TO_BIN(time.tm_sec);
	BCD_TO_BIN(time.tm_min);
	BCD_TO_BIN(time.tm_hour);
	BCD_TO_BIN(time.tm_mday);
	BCD_TO_BIN(time.tm_mon);
	BCD_TO_BIN(time.tm_year);

	dprintk_color("[current time] ", 0x0e);
	dprintf("%-", time.tm_year);
	dprintf("%-", time.tm_mon);
	dprintf("%-", time.tm_mday);
	dprintf("%:", time.tm_hour);
	dprintf("%:", time.tm_min);
	dprintf("%\n", time.tm_sec);
	startup_time = kernel_mktime(&time);
}

int kernel_start() {

	debug_init();
	dprintk("debug init finish\n");
	trap_init();
	dprintk("trap init finish\n");
	keyboard_init();
	dprintk("keyboard init finish\n");
	hd_init();
	dprintk("hd init finish\n");
	time_init();
	dprintf("time init finish\n", startup_time);


	dprintk("\n");

	sti();
	hd_out(1, 1, 0, 0, 0x30);
	outsw(PORT_DISK0_DATA, &write_buf, 256);
	hd_out(1, 1, 0, 0, 0x20);

	dprintk("\n");
	dprintk_color("[input]", 0x0e);

	//dprintf("do_hd_read: %\n", (int)&do_hd_read);
	//dprintf("do_hd_write: %\n", (int)&do_hd_write);
	//dprintf("hd_interrupt: %\n", hd_interrupt);

	// 系统怠速
	for (;;) {
		count++;
	}
}

/*
* 表 9–3 AT 硬盘控制器寄存器端口及作用
*	端口		名称					读操作							写操作
*	0x1f0	HD_DATA				数据寄存器 -- 扇区数据（读、写、格式化）
*	0x1f1	HD_ERROR,HD_PRECOMP 错误寄存器（错误状态）(HD_ERROR)		写前预补偿寄存器 (HD_PRECOMP)
*	0x1f2	HD_NSECTOR			扇区数寄存器 -- 扇区数（读、写、检验、格式化）
*	0x1f3	HD_SECTOR			扇区号寄存器 -- 起始扇区（读、写、检验）
*	0x1f4	HD_LCYL				柱面号寄存器 -- 柱面号低字节（读、写、检验、格式化）
*	0x1f5	HD_HCYL				柱面号寄存器 -- 柱面号高字节（读、写、检验、格式化）
*	0x1f6	HD_CURRENT			驱动器/磁头寄存器 -- 驱动器号/磁头号(101dhhhh, d=驱动器号,h=磁头号)
*	0x1f7	HD_STATUS,HD_COMMAND 主状态寄存器 (HD_STATUS)			命令寄存器 (HD_COMMAND)
*	0x3f6	HD_CMD ---											硬盘控制寄存器 (HD_CMD)
*	0x3f7						数字输入寄存器（与 1.2M 软盘合用） ---
*/

/*
* 表 9–5 驱动器/磁头寄存器含义
*	位	名称			说明
*	0	HS0			磁头号位 0 磁头号最低位。
*	1	HS1			磁头号位 1
*	2	HS2			磁头号位 2
*	3	HS3			磁头号位 3 磁头号最高位。
*	4	DRV			驱动器 选择驱动器，0 - 选择驱动器 0； 1 - 选择驱动器 1。
*	5	Reserved	保留 总是 1。
*	6	Reserved	保留 总是 0。
*	7	Reserved	保留 总是 1。
*/

#define PIC_M_CTRL 0x20	       // 这里用的可编程中断控制器是8259A,主片的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从片的数据端口是0xa1

#define PORT_DISK0_DATA				0x1f0
#define PORT_DISK0_ERR_FEATURE		0x1f1
#define PORT_DISK0_SECTOR_CNT		0x1f2
#define PORT_DISK0_SECTOR_LOW		0x1f3
#define PORT_DISK0_SECTOR_MID		0x1f4
#define PORT_DISK0_SECTOR_HIGH		0x1f5
#define PORT_DISK0_DEVICE			0x1f6
#define PORT_DISK0_STATUS_CMD		0x1f7

#define PORT_DISK0_ALT_STA_CTL		0x3f6

#define PORT_DISK1_DATA				0x170
#define PORT_DISK1_ERR_FEATURE		0x171
#define PORT_DISK1_SECTOR_CNT		0x172
#define PORT_DISK1_SECTOR_LOW		0x173
#define PORT_DISK1_SECTOR_MID		0x174
#define PORT_DISK1_SECTOR_HIGH		0x175
#define PORT_DISK1_DEVICE			0x176
#define PORT_DISK1_STATUS_CMD		0x177

#define PORT_DISK1_ALT_STA_CTL		0x376

#define DISK_STATUS_BUSY	(1<<7)
#define DISK_STATUS_READY	(1<<6)
#define DISK_STATUS_SEEK	(1<<4)
#define DISK_STATUS_REQ		(1<<3)
#define DISK_STATUS_ERROR	(1<<0)

void test_ide() {
	dprintf("ide num: %\n", *((char*)0x9100c));

	outb(0xf8, 0x21);	
	outb(0xbf, 0xA1);		//开启

	//while (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_BUSY);

	dprintf("disk not busy status: %\n", inb(PORT_DISK0_STATUS_CMD));
	
	outb(0xe0, PORT_DISK0_DEVICE);

	outb(0, PORT_DISK0_ERR_FEATURE);
	outb(1, PORT_DISK0_SECTOR_CNT);
	outb(0, PORT_DISK0_SECTOR_LOW);
	outb(0, PORT_DISK0_SECTOR_MID);
	outb(0, PORT_DISK0_SECTOR_HIGH);

	//while (!inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_READY);

	//dprintf("disk ready status: %\n", inb(PORT_DISK0_STATUS_CMD));

	//dprintf("do_hd_read: %\n", (int)&do_hd_read);
	//dprintf("do_hd_write: %\n", (int)&do_hd_write);

	//outb(0x20, PORT_DISK0_STATUS_CMD);
	//outb(0x30, PORT_DISK0_STATUS_CMD);

}

