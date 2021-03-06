# $Id: //depot/sw/releases/Aquila_9.2.0_U8/drivers/wlan/os/linux/ath_hal/Makefile#1 $

#
# Makefile for the Atheros HAL Module.
#
# NB: we can't support MODVERSIONS because it requires building
#     from source which may not be available
#
ifeq ($(obj),)
obj := .
endif

DEPTH := ../../..
ADF := ${obj}/${DEPTH}/adf
ASF := ${obj}/${DEPTH}/asf

obj-m		+= ath_hal.o
ath_hal-objs	:= ah_adf.o hal.o
ifeq ($(findstring 2.6,$(KERNELRELEASE)),)
export-objs	:= ah_adf.o
list-multi	:= ath_hal.o
endif

# If building into kernel

ifneq ($(MAKING_MODULES),1)
obj-$(CONFIG_ATHEROS_HAL) := ah_adf.o hal.o
ifeq ($(findstring 2.6,$(KERNELRELEASE)),)
O_TARGET	:= ath_hal.o
endif
endif

include $(obj)/${DEPTH}/os/linux/Makefile.inc

UUDECODE?=uudecode

ifeq ($(ATH_SUPPORT_HTC),1)
INCS=	-I${HAL} -I${HAL}/${OS} -I${obj}/${ATH_HAL} -I${obj}/${DEPTH} -I${COMMON} -I${ATH_RATE} -I${ATH}
else
INCS+=	-I${HAL} -I${HAL}/${OS} -I${obj}/${ATH_HAL} -I${obj}/${DEPTH} -I${COMMON} -I${ATH_RATE} -I${ATH}
endif
# -I${WLAN} 
INCS+=  -I${obj}/$(DEPTH)/include
INCS+=  -I$(ADF)/include/os -I$(ADF)/include/nbuf -I$(ADF)/include/net
INCS+=  -I$(ADF)/os/linux -I$(ADF)/nbuf/linux -I$(ADF)/net/linux
INCS+=  -I$(ASF)/include

EXTRA_CFLAGS+= ${INCS} ${COPTS} -DBUILD_HAL

-include $(WLAN_TOP)/Rules.make

all:	ah_adf.c hal.o opt_ah.h
ifneq ($(BUILD_X86),y)
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(shell pwd) modules
else
	$(MAKE) -C $(KERNELPATH) KBUILD_EXTRA_SYMBOLS=$(PWD)/../Module.symvers SUBDIRS=$(shell pwd) modules
endif

ah_adf.c: opt_ah.h
#	cp -f $(obj)/${DEPTH}/os/${OS}/ah_adf.c ah_osdep.c

$(obj)/hal.o:${HAL}/${OS}/obj/${TARGET}/hal.o
	cp ${HAL}/${OS}/obj/${TARGET}/hal.o .
#${UUDECODE} ${HAL}/public/${TARGET}.hal.o.uu

opt_ah.h: ${HAL}/${OS}/public/${TARGET}.opt_ah.h
	cp -f ${HAL}/${OS}/public/${TARGET}.opt_ah.h opt_ah.h

# Copy the unstripped version of the driver to /tftpboot/`whoami`/drivers/
# if debug is enabled.
install: all
ifeq ($(AP71_DEBUG),1)
	test -d ${TFTPPATH}/drivers || mkdir -p ${TFTPPATH}/drivers
	cp ath_hal.${KMODSUF} ${TFTPPATH}/drivers/
endif
	test -d ${DESTDIR}/${MODULEPATH} || mkdir -p ${DESTDIR}/${MODULEPATH}
	$(STRIP) -S ath_hal.$(KMODSUF)
	cp ath_hal.${KMODSUF} ${DESTDIR}/${MODULEPATH}

clean:
	rm -f *~ *.o *.ko *.mod.c
	rm -f .depend .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd
# remove the files we copied to this dir
	rm -f opt_ah.h hal.o

ath_hal.o: ${ath_hal-objs}
	$(LD) $(LDOPTS) -o ath_hal.$(KMODSUF) -r $(ath_hal-objs)
