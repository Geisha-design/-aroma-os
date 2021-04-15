#include "trap.h"
#include "debug.h"
#include "asm/io.h"
#include "asm/system.h"

extern void reserved_interrupt_entry0(void);
extern void reserved_interrupt_entry1(void);
extern void reserved_interrupt_entry2(void);
extern void reserved_interrupt_entry3(void);
extern void reserved_interrupt_entry4(void);
extern void reserved_interrupt_entry5(void);
extern void reserved_interrupt_entry6(void);
extern void reserved_interrupt_entry7(void);
extern void reserved_interrupt_entry8(void);
extern void reserved_interrupt_entry9(void);
extern void reserved_interrupt_entry10(void);
extern void reserved_interrupt_entry11(void);
extern void reserved_interrupt_entry12(void);
extern void reserved_interrupt_entry13(void);
extern void reserved_interrupt_entry14(void);
extern void reserved_interrupt_entry15(void);
extern void reserved_interrupt_entry16(void);
extern void reserved_interrupt_entry17(void);
extern void reserved_interrupt_entry18(void);
extern void reserved_interrupt_entry19(void);
extern void reserved_interrupt_entry20(void);
extern void reserved_interrupt_entry21(void);
extern void reserved_interrupt_entry22(void);
extern void reserved_interrupt_entry23(void);
extern void reserved_interrupt_entry24(void);
extern void reserved_interrupt_entry25(void);
extern void reserved_interrupt_entry26(void);
extern void reserved_interrupt_entry27(void);
extern void reserved_interrupt_entry28(void);
extern void reserved_interrupt_entry29(void);
extern void reserved_interrupt_entry30(void);
extern void reserved_interrupt_entry31(void);
extern void reserved_interrupt_entry32(void);
extern void reserved_interrupt_entry33(void);
extern void reserved_interrupt_entry34(void);
extern void reserved_interrupt_entry35(void);
extern void reserved_interrupt_entry36(void);
extern void reserved_interrupt_entry37(void);
extern void reserved_interrupt_entry38(void);
extern void reserved_interrupt_entry39(void);
extern void reserved_interrupt_entry40(void);
extern void reserved_interrupt_entry41(void);
extern void reserved_interrupt_entry42(void);
extern void reserved_interrupt_entry43(void);
extern void reserved_interrupt_entry44(void);
extern void reserved_interrupt_entry45(void);
extern void reserved_interrupt_entry46(void);
extern void reserved_interrupt_entry47(void);
extern void reserved_interrupt_entry48(void);
extern void reserved_interrupt_entry49(void);

static char a[512];

/* 保留中断，暂时全写成保留中断，哈哈 */
void reserved_interrupt(int intr_num) {
	dprintf("reserved_interrupt occur:%\n", intr_num);
	
	if (intr_num == 0x20) {
		insw(0x1f0, &a, 256);
	}

	for (int i = 0; i < 512; i++) {
		dprintf("%", a[i]);
	}
}

void trap_init() {

	set_trap_gate(0, &reserved_interrupt_entry0);
	set_trap_gate(1, &reserved_interrupt_entry1);
	set_trap_gate(2, &reserved_interrupt_entry2);
	set_trap_gate(3, &reserved_interrupt_entry3);
	set_trap_gate(4, &reserved_interrupt_entry4);
	set_trap_gate(5, &reserved_interrupt_entry5);
	set_trap_gate(6, &reserved_interrupt_entry6);
	set_trap_gate(7, &reserved_interrupt_entry7);
	set_trap_gate(8, &reserved_interrupt_entry8);
	set_trap_gate(9, &reserved_interrupt_entry9);
	set_trap_gate(10, &reserved_interrupt_entry10);
	set_trap_gate(11, &reserved_interrupt_entry11);
	set_trap_gate(12, &reserved_interrupt_entry12);
	set_trap_gate(13, &reserved_interrupt_entry13);
	set_trap_gate(14, &reserved_interrupt_entry14);
	set_trap_gate(15, &reserved_interrupt_entry15);
	set_trap_gate(16, &reserved_interrupt_entry16);
	set_trap_gate(17, &reserved_interrupt_entry17);
	set_trap_gate(18, &reserved_interrupt_entry18);
	set_trap_gate(19, &reserved_interrupt_entry19);
	set_trap_gate(20, &reserved_interrupt_entry20);
	set_trap_gate(21, &reserved_interrupt_entry21);
	set_trap_gate(22, &reserved_interrupt_entry22);
	set_trap_gate(23, &reserved_interrupt_entry23);
	set_trap_gate(24, &reserved_interrupt_entry24);
	set_trap_gate(25, &reserved_interrupt_entry25);
	set_trap_gate(26, &reserved_interrupt_entry26);
	set_trap_gate(27, &reserved_interrupt_entry27);
	set_trap_gate(28, &reserved_interrupt_entry28);
	set_trap_gate(29, &reserved_interrupt_entry29);
	set_trap_gate(30, &reserved_interrupt_entry30);
	set_trap_gate(31, &reserved_interrupt_entry31);
	set_trap_gate(32, &reserved_interrupt_entry32);
	set_trap_gate(33, &reserved_interrupt_entry33);
	set_trap_gate(34, &reserved_interrupt_entry34);
	set_trap_gate(35, &reserved_interrupt_entry35);
	set_trap_gate(36, &reserved_interrupt_entry36);
	set_trap_gate(37, &reserved_interrupt_entry37);
	set_trap_gate(38, &reserved_interrupt_entry38);
	set_trap_gate(39, &reserved_interrupt_entry39);
	set_trap_gate(40, &reserved_interrupt_entry40);
	set_trap_gate(41, &reserved_interrupt_entry41);
	set_trap_gate(42, &reserved_interrupt_entry42);
	set_trap_gate(43, &reserved_interrupt_entry43);
	set_trap_gate(44, &reserved_interrupt_entry44);
	set_trap_gate(45, &reserved_interrupt_entry45);
	set_trap_gate(46, &reserved_interrupt_entry46);
	set_trap_gate(47, &reserved_interrupt_entry47);
	set_trap_gate(48, &reserved_interrupt_entry48);
	set_trap_gate(49, &reserved_interrupt_entry49);

}

