#include "trap.h"
#include "debug.h"
#include "asm/io.h"
#include "asm/system.h"
#include "keyboard.h"

#define KBD_BUF_PORT 0x60	 // ����buffer�Ĵ����˿ں�Ϊ0x60

/* ��ת���ַ����岿�ֿ����ַ� */
#define esc			'\033'	 // �˽��Ʊ�ʾ�ַ�,Ҳ������ʮ������'\x1b'
#define backspace	'\b'
#define tab			'\t'
#define enter		'\r'
#define delete		'\177'	 // �˽��Ʊ�ʾ�ַ�,ʮ������Ϊ'\x7f'

/* ���ϲ��ɼ��ַ�һ�ɶ���Ϊ0 */
#define char_invisible	0
#define ctrl_l_char		char_invisible
#define ctrl_r_char		char_invisible
#define shift_l_char	char_invisible
#define shift_r_char	char_invisible
#define alt_l_char		char_invisible
#define alt_r_char		char_invisible
#define caps_lock_char	char_invisible

/* ��������ַ���ͨ��Ͷ��� */
#define shift_l_make	0x2a
#define shift_r_make 	0x36 
#define alt_l_make   	0x38
#define alt_r_make   	0xe038
#define alt_r_break   	0xe0b8
#define ctrl_l_make  	0x1d
#define ctrl_r_make  	0xe01d
#define ctrl_r_break 	0xe09d
#define caps_lock_make 	0x3a

/* �������±�����¼��Ӧ���Ƿ��µ�״̬,
 * ext_scancode���ڼ�¼makecode�Ƿ���0xe0��ͷ */
static int ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

/* ��ͨ��make_codeΪ�����Ķ�ά���� */
static char keymap[][2] = {
	/* ɨ����   δ��shift���  ��shift���*/
	/* ---------------------------------- */
	/* 0x00 */	{0,	0},
	/* 0x01 */	{esc,	esc},
	/* 0x02 */	{'1',	'!'},
	/* 0x03 */	{'2',	'@'},
	/* 0x04 */	{'3',	'#'},
	/* 0x05 */	{'4',	'$'},
	/* 0x06 */	{'5',	'%'},
	/* 0x07 */	{'6',	'^'},
	/* 0x08 */	{'7',	'&'},
	/* 0x09 */	{'8',	'*'},
	/* 0x0A */	{'9',	'('},
	/* 0x0B */	{'0',	')'},
	/* 0x0C */	{'-',	'_'},
	/* 0x0D */	{'=',	'+'},
	/* 0x0E */	{backspace, backspace},
	/* 0x0F */	{tab,	tab},
	/* 0x10 */	{'q',	'Q'},
	/* 0x11 */	{'w',	'W'},
	/* 0x12 */	{'e',	'E'},
	/* 0x13 */	{'r',	'R'},
	/* 0x14 */	{'t',	'T'},
	/* 0x15 */	{'y',	'Y'},
	/* 0x16 */	{'u',	'U'},
	/* 0x17 */	{'i',	'I'},
	/* 0x18 */	{'o',	'O'},
	/* 0x19 */	{'p',	'P'},
	/* 0x1A */	{'[',	'{'},
	/* 0x1B */	{']',	'}'},
	/* 0x1C */	{enter,  enter},
	/* 0x1D */	{ctrl_l_char, ctrl_l_char},
	/* 0x1E */	{'a',	'A'},
	/* 0x1F */	{'s',	'S'},
	/* 0x20 */	{'d',	'D'},
	/* 0x21 */	{'f',	'F'},
	/* 0x22 */	{'g',	'G'},
	/* 0x23 */	{'h',	'H'},
	/* 0x24 */	{'j',	'J'},
	/* 0x25 */	{'k',	'K'},
	/* 0x26 */	{'l',	'L'},
	/* 0x27 */	{';',	':'},
	/* 0x28 */	{'\'',	'"'},
	/* 0x29 */	{'`',	'~'},
	/* 0x2A */	{shift_l_char, shift_l_char},
	/* 0x2B */	{'\\',	'|'},
	/* 0x2C */	{'z',	'Z'},
	/* 0x2D */	{'x',	'X'},
	/* 0x2E */	{'c',	'C'},
	/* 0x2F */	{'v',	'V'},
	/* 0x30 */	{'b',	'B'},
	/* 0x31 */	{'n',	'N'},
	/* 0x32 */	{'m',	'M'},
	/* 0x33 */	{',',	'<'},
	/* 0x34 */	{'.',	'>'},
	/* 0x35 */	{'/',	'?'},
	/* 0x36	*/	{shift_r_char, shift_r_char},
	/* 0x37 */	{'*',	'*'},
	/* 0x38 */	{alt_l_char, alt_l_char},
	/* 0x39 */	{' ',	' '},
	/* 0x3A */	{caps_lock_char, caps_lock_char}
	/*���������ݲ�����*/
};

extern void keyboard_interrupt_entry(void);

void keyboard_init() {
	register unsigned char a;

	set_trap_gate(0x21, &keyboard_interrupt_entry);			// ���ü����ж������š�
	outb_p((unsigned char)(inb_p(0x21) & 0xfd), 0x21);	// ȡ��8259A �жԼ����жϵ����Σ�����IRQ1��
	a = inb_p(0x61);									// �ӳٶ�ȡ���̶˿�0x61(8255A �˿�PB)��
	outb_p((unsigned char)(a | 0x80), 0x61);			// ���ý�ֹ���̹���(λ7 ��λ)��
	outb(a, 0x61);										// ��������̹��������Ը�λ���̲�����

}

void keyboard_interrupt(int intr_num) {
	/* ����жϷ���ǰ����һ���ж�,���������������Ƿ��а��� */
	int ctrl_down_last = ctrl_status;
	int shift_down_last = shift_status;
	int caps_lock_last = caps_lock_status;

	int break_code;
	int scancode = inb(KBD_BUF_PORT);
	dprint_info("      <-- keyboard_interrupt");
	dprint_info_hex(scancode, 0);

	/* ��ɨ������e0��ͷ��,��ʾ�˼��İ��½��������ɨ����,
	 * �������Ͻ����˴��жϴ�����,�ȴ���һ��ɨ�������*/
	if (scancode == 0xe0) {
		ext_scancode = 1;    // ��e0���
		return;
	}

	/* ����ϴ�����0xe0��ͷ,��ɨ����ϲ� */
	if (ext_scancode) {
		scancode = ((0xe000) | scancode);
		ext_scancode = 0;   // �ر�e0���
	}

	break_code = ((scancode & 0x0080) != 0);   // ��ȡbreak_code

	if (break_code) {   // ���Ƕ���break_code(��������ʱ������ɨ����)

	/* ����ctrl_r ��alt_r��make_code��break_code�������ֽ�,
	���Կ�������ķ���ȡmake_code,���ֽڵ�ɨ�����ݲ����� */
		int make_code = (scancode &= 0xff7f);   // �õ���make_code(��������ʱ������ɨ����)

	 /* ������������������������,��״̬��Ϊfalse */
		if (make_code == ctrl_l_make || make_code == ctrl_r_make) {
			ctrl_status = 0;
		} else if (make_code == shift_l_make || make_code == shift_r_make) {
			shift_status = 0;
		} else if (make_code == alt_l_make || make_code == alt_r_make) {
			alt_status = 0;
		} /* ����caps_lock���ǵ����ر�,������Ҫ�������� */

		return;   // ֱ�ӷ��ؽ����˴��жϴ������

	}
	/* ��Ϊͨ��,ֻ���������ж���ļ��Լ�alt_right��ctrl��,ȫ��make_code */
	else if ((scancode > 0x00 && scancode < 0x3b) || \
		(scancode == alt_r_make) || \
		(scancode == ctrl_r_make)) {
		int shift = 0;  // �ж��Ƿ���shift���,������һά������������Ӧ���ַ�
		if ((scancode < 0x0e) || (scancode == 0x29) || \
			(scancode == 0x1a) || (scancode == 0x1b) || \
			(scancode == 0x2b) || (scancode == 0x27) || \
			(scancode == 0x28) || (scancode == 0x33) || \
			(scancode == 0x34) || (scancode == 0x35)) {
			/****** ����������ĸ�ļ� ********
				 0x0e ����'0'~'9',�ַ�'-',�ַ�'='
				 0x29 �ַ�'`'
				 0x1a �ַ�'['
				 0x1b �ַ�']'
				 0x2b �ַ�'\\'
				 0x27 �ַ�';'
				 0x28 �ַ�'\''
				 0x33 �ַ�','
				 0x34 �ַ�'.'
				 0x35 �ַ�'/'
			*******************************/
			if (shift_down_last) {  // ���ͬʱ������shift��
				shift = 1;
			}
		} else {	  // Ĭ��Ϊ��ĸ��
			if (shift_down_last && caps_lock_last) {  // ���shift��capslockͬʱ����
				shift = 0;
			} else if (shift_down_last || caps_lock_last) { // ���shift��capslock���ⱻ����
				shift = 1;
			} else {
				shift = 0;
			}
		}

		char index = (scancode &= 0x00ff);  // ��ɨ����ĸ��ֽ���0,��Ҫ����Ը��ֽ���e0��ɨ����.
		char cur_char = keymap[index][shift];  // ���������ҵ���Ӧ���ַ�

		if (cur_char == enter) {
			dprintk("\n");
		} else {
			dprintc(cur_char);
		}

		

	} else {
		dprintk("unknown key\n");
	}

}

