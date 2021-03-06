export TOPDIR=$(PWD)/
export TOOLPATH=$(TOPDIR)/../../build/gcc-4.3.3/build_mips/staging_dir/usr/
export KERNELPATH=$(TOPDIR)/../../linux/kernels/mips-linux-2.6.31
export UTILPATH=$(TOPDIR)/../../boot/utility/bin/

export PATH:=$(TOPDIR)/build/util:$(TOPDIR)/tools:$(TOPDIR)/src:$(TOOLPATH)/bin:$(TOPDIR)/linux:$(TOPDIR)/build:$(BOOTLOADERDIR)/ecos/tools/bin:`pwd`:${PATH}


PRJ_NAME = fw

######## CONFIGURATION #########
# RAM version
LOADADDR = 0x81200000

CROSS_COMPILE = mips-linux-uclibc-

#for decompress code
OBJS = start.o main.o LzmaDecode.o LzmaWrapper.o printf.o serial.o

######## END OF CONFIGURATION #########

.S.s:
	$(CPP) $(AFLAGS) $< -o $*.s
.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

CC =       $(CROSS_COMPILE)gcc
LD =       $(CROSS_COMPILE)ld
OBJCOPY =  $(CROSS_COMPILE)objcopy
OBJDUMP =  $(CROSS_COMPILE)objdump

AFLAGS = -D__ASSEMBLY__ -DTEXT_BASE=0x80400000 -I$(TOPDIR) -I$(TOPDIR)/include -fno-builtin -nostdinc -DCONFIG_MIPS -D__MIPS__ -march=74kc -mno-abicalls -pipe -fno-pic
CFLAGS = -Wall -Wa,-gstabs -g  -O1   -D__KERNEL__ -I$(TOPDIR) -I$(TOPDIR)/include -fno-builtin -nostdinc -pipe  -DCONFIG_MIPS -D__MIPS__  -march=74kc -mno-abicalls -fno-pic

# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
drop-sections   = .reginfo .mdebug
strip-flags     = $(addprefix --remove-section=,$(drop-sections))


all : elf srec bin

x86_test:
	gcc main.c LzmaDecode.c LzmaWrapper.c vmlrd.c -o decompress

bin2barray: bin2barray.c
	gcc bin2barray.c -o bin2barray

.PHONY: vmlrd_lzma.c

vmlrd_lzma.c: bin2barray vmlrd.bin
	lzma e vmlrd.bin -so > vmlrd.bin.lzma
	./bin2barray vmlrd.bin.lzma > vmlrd_lzma.c
	cat vmlinux.info | sed "s/:/ \=/" | sed 's/^/unsigned long vmlrd_lzma_/' | sed 's/$$/\;/' >> vmlrd_lzma.c

ihex : $(PRJ_NAME).ihex
srec : $(PRJ_NAME).srec
elf : $(PRJ_NAME).elf
bin : $(PRJ_NAME).bin
fulldump : $(PRJ_NAME).objdump

$(PRJ_NAME).elf : $(OBJS) vmlrd_lzma.o
	$(LD) -o $(PRJ_NAME).elf -N -Tlinker.lds -Ttext $(LOADADDR) $^

$(PRJ_NAME).srec : $(PRJ_NAME).elf
	$(OBJCOPY) -S -O srec $(strip-flags) $(PRJ_NAME).elf $(PRJ_NAME).srec

$(PRJ_NAME).ihex : $(PRJ_NAME).elf
	$(OBJCOPY) -I binary -O ihex $(PRJ_NAME).bin $(PRJ_NAME).ihex

$(PRJ_NAME).bin : $(PRJ_NAME).elf
	$(OBJCOPY) -S -O binary $(strip-flags) $(PRJ_NAME).elf $(PRJ_NAME).bin
	$(UTILPATH)/bin2hex $(PRJ_NAME).bin vmlrd_lzma.c 0 0 vmlrd_lzma
    
$(PRJ_NAME).objdump:  $(PRJ_NAME).elf
	$(OBJDUMP) -Dlxs $< > $@

clean:
	rm -f *~ *.o $(PRJ_NAME).elf $(PRJ_NAME).srec $(PRJ_NAME).bin $(PRJ_NAME).objdump *.ihex *.out *.bin *.info bin2barray vmlrd_lzma.c *.lzma
