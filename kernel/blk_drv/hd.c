/*
* 本程序是底层硬盘中断辅助程序。主要用于扫描请求列表，使用中断在函数之间跳转。
* 由于所有的函数都是在中断里调用的，所以这些函数不可以睡眠。请特别注意。
* 由Drew Eckhardt 修改，利用CMOS 信息检测硬盘数。
*/

#include <hdreg.h>			// 硬盘参数头文件。定义访问硬盘寄存器端口，状态码，分区表等信息。
#include <asm/system.h>		// 系统头文件。定义了设置或修改描述符/中断门等的嵌入式汇编宏。
#include <asm/io.h>		// io 头文件。定义硬件端口输入/输出宏汇编语句。
#include <debug.h>	// 段操作头文件。定义了有关段寄存器操作的嵌入式汇编函数。

#define PORT_DISK0_DATA				0x1f0
#define PORT_DISK0_ERR_FEATURE		0x1f1
#define PORT_DISK0_SECTOR_CNT		0x1f2
#define PORT_DISK0_SECTOR_LOW		0x1f3
#define PORT_DISK0_SECTOR_MID		0x1f4
#define PORT_DISK0_SECTOR_HIGH		0x1f5
#define PORT_DISK0_DEVICE			0x1f6
#define PORT_DISK0_STATUS_CMD		0x1f7

#define PORT_DISK0_ALT_STA_CTL		0x3f6

#define DISK_STATUS_BUSY	(1<<7)
#define DISK_STATUS_READY	(1<<6)
#define DISK_STATUS_SEEK	(1<<4)
#define DISK_STATUS_REQ		(1<<3)
#define DISK_STATUS_ERROR	(1<<0)

static char buf_read[512] = { 'y', 'd', };
static char buf_write[512] = { 'y', 'd', };

void do_hd_read(int intr_num) {
	dprintf_color("do_hd_read occur:%\n", intr_num);
	insw(PORT_DISK0_DATA, &buf_read, 256);
}

void do_hd_write(int intr_num) {
	dprintf_color("do_hd_write occur:%\n", intr_num);
	outsw(PORT_DISK0_DATA, &buf_write, 256);
}

int hd_interrupt = &do_hd_read;

//// 向硬盘控制器发送命令块（参见列表后的说明）。
// 调用参数：drive - 硬盘号(0-1)； nsect - 读写扇区数；
// sect - 起始扇区； head - 磁头号；
// cyl - 柱面号； cmd - 命令码；
// *intr_addr() - 硬盘中断处理程序中将调用的C 处理函数。
void hd_out(unsigned int count, unsigned int sect, unsigned int head, unsigned int cyl, unsigned int cmd) {
	register int port;
	while (inb(PORT_DISK0_STATUS_CMD) & DISK_STATUS_BUSY);

	if (cmd == 0x20) {
		hd_interrupt = &do_hd_read;
	}

	if (cmd == 0x30) {
		hd_interrupt = &do_hd_write;
	}

	outb(0xb0 | head, PORT_DISK0_DEVICE);
	outb(0, PORT_DISK0_ERR_FEATURE);
	outb(count, PORT_DISK0_SECTOR_CNT);
	outb(sect, PORT_DISK0_SECTOR_LOW);
	outb(cyl, PORT_DISK0_SECTOR_MID);
	outb(cyl >> 8, PORT_DISK0_SECTOR_HIGH);
	outb(cmd, PORT_DISK0_STATUS_CMD);
}

extern void hd_interrupt_entry;

// 硬盘系统初始化。
void hd_init(void) {
	set_intr_gate(0x2e, &hd_interrupt_entry);	// 设置硬盘中断门向量 int 0x2E(46)。
	outb_p(inb_p(0x21) & 0xfb, 0x21);	// 复位接联的主8259A int2 的屏蔽位，允许从片发出中断请求信号。
	outb(inb_p(0xA1) & 0xbf, 0xA1);	// 复位硬盘的中断请求屏蔽位（在从片上），允许硬盘控制器发送中断请求信号。
}
