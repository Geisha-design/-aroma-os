/*
* �������ǵײ�Ӳ���жϸ���������Ҫ����ɨ�������б�ʹ���ж��ں���֮����ת��
* �������еĺ����������ж�����õģ�������Щ����������˯�ߡ����ر�ע�⡣
* ��Drew Eckhardt �޸ģ�����CMOS ��Ϣ���Ӳ������
*/

#include <hdreg.h>			// Ӳ�̲���ͷ�ļ����������Ӳ�̼Ĵ����˿ڣ�״̬�룬���������Ϣ��
#include <asm/system.h>		// ϵͳͷ�ļ������������û��޸�������/�ж��ŵȵ�Ƕ��ʽ���ꡣ
#include <asm/io.h>		// io ͷ�ļ�������Ӳ���˿�����/���������䡣
#include <debug.h>	// �β���ͷ�ļ����������йضμĴ���������Ƕ��ʽ��ຯ����

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

//// ��Ӳ�̿�������������飨�μ��б���˵������
// ���ò�����drive - Ӳ�̺�(0-1)�� nsect - ��д��������
// sect - ��ʼ������ head - ��ͷ�ţ�
// cyl - ����ţ� cmd - �����룻
// *intr_addr() - Ӳ���жϴ�������н����õ�C ��������
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

// Ӳ��ϵͳ��ʼ����
void hd_init(void) {
	set_intr_gate(0x2e, &hd_interrupt_entry);	// ����Ӳ���ж������� int 0x2E(46)��
	outb_p(inb_p(0x21) & 0xfb, 0x21);	// ��λ��������8259A int2 ������λ�������Ƭ�����ж������źš�
	outb(inb_p(0xA1) & 0xbf, 0xA1);	// ��λӲ�̵��ж���������λ���ڴ�Ƭ�ϣ�������Ӳ�̿����������ж������źš�
}
