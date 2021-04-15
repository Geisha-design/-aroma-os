include Rules.make

MAIN_O =			\
	boot/head.o		\
	init/main.o		\

DEBUG_O =			\
	debug/dprintk.o	\

KERNEL_O =				\
	kernel/trap.o		\
	kernel/keyboard.o	\
	kernel/intr.o		\
	kernel/ide.o		\
	kernel/vsprintf.o	\
	kernel/mktime.o	\
	kernel/blk_drv/hd.o	\

OBJECTS = $(MAIN_O) $(DEBUG_O) $(KERNEL_O)

all: Image

Image: os.raw file.raw
	@echo [ӳ��׼������] �ں�ӳ�� oa.raw �ļ�ϵͳӳ�� file.raw

os.raw: others/bochs/os.disk boot/bootsect.bin boot/setup.bin init/main.bin
	@echo [��������ں˵�����Ӳ��] ׼�����ļ� os.raw
	@copy /Y others\bochs\os.disk os.raw
	@echo [��������ں˵�����Ӳ��] ��1��������������д�� bootsect.bin
	@dd if=boot/bootsect.bin of=os.raw bs=512 count=1
	@echo [��������ں˵�����Ӳ��] ��2����д������4�������� setup.bin
	@dd if=boot/setup.bin of=os.raw bs=512 count=4 seek=1
	@echo [��������ں˵�����Ӳ��] ��5������ʼд���Ӵ���ں��ļ� main.bin
	@dd if=init/main.bin of=os.raw bs=512 count=100 seek=5
	@echo [��������ں˵�����Ӳ��] os.raw �������
	
file.raw: others/bochs/file.disk
	@echo [��������ļ�ϵͳ������Ӳ��] ׼�����ļ� file.raw
	@copy /Y others\bochs\file.disk file.raw
	@echo [��������ļ�ϵͳ������Ӳ��] file.raw �������

######### �������ļ���������(bootsect)��������(setup)���ں�(main) #########
	
boot/bootsect.bin: boot/bootsect.s
	@echo [��������������] bootsect.bin
	@nasm -I include/ -o boot/bootsect.bin boot/bootsect.s -l boot/bootsect.lst
	
boot/setup.bin: boot/setup.s
	@echo [�������ɼ�����] setup.bin
	@nasm -I include/ -o boot/setup.bin boot/setup.s -l boot/setup.lst

init/main.bin: $(OBJECTS)
	@echo [�����������յ��ںˣ�������] main.bin
	@$(LD) $(OBJECTS) -Ttext 0x0 -e startup_32 -o init/main.bin.large
	@objcopy -O binary init/main.bin.large init/main.bin


######### �ں˲��ֵĸ���Ŀ���ļ� #########

%.o:%.c
	@echo ...[��������Ŀ���ļ� cд��] $@
	@gcc $(LIB) $(GCCPARAM) -o $@ $<

%.o:%.s
	@echo ...[��������Ŀ���ļ� nasmд��] $@
	@nasm -f elf -I include/ -o $@ $<

######### �������� #########

run: Image
	@echo [��bochs�����ں�] ����ģʽ����
	@bochs -f $(BOCHS_HOME)/bochsrc.disk -q
	
brun: Image
	@echo [��bochs�����ں�] ����ģʽ����
	@bochsdbg -f $(BOCHS_HOME)/bochsrc.disk -q
	
clean:
	@echo ������.....
	@del *.raw*
	@cd boot && make clean
	@cd debug && make clean
	@cd init && make clean
	@cd kernel && make clean