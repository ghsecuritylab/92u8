#include <fwd.h>
#include <asm/types.h>
#include <asm/addrspace.h>
#include <ar934x_soc.h>
#include "rom.addrs.h"

void (*serial_puts) (const char *s);
int (*printf) (const char *fmt, ...);
void (*udelay) (unsigned long usec);
void (*simple_mips_cache_reset)(void);
void (*call_fw)(u32, u32);

void
drv_main(void)
{
	int	interface, ver;
	extern	bootrom_fn_t bootrom_fn[];

	ver = ar7240_reg_rd(RST_REVISION_ID_ADDRESS) & 0xf;

	serial_puts		= bootrom_fn[ver].serial_puts;
    printf     = bootrom_fn[ver].printf;
	udelay			= bootrom_fn[ver].udelay;
	simple_mips_cache_reset	= bootrom_fn[ver].simple_mips_cache_reset;
	call_fw			= bootrom_fn[ver].call_fw;

	simple_mips_cache_reset();
	interface = WASP_BOOT_TYPE(ar7240_reg_rd(WASP_BOOTSTRAP_REG));
#if 1	/* used for debug */
	serial_puts("initialize PLL & DDR\n");
	ar934x_lowlevel_init();
	wasp_mem_config(0);
	if (interface == HIF_USB) {
		/*
		 * USB Boot-ROM shutdown RC, EP and Supercore.
		 * Hence, bring them up here so that 2nd stage
		 * s/w can use them.
		 */
		ar7240_reg_wr(0x18116d00, 0x1021265e);
		ar7240_reg_wr(0x18116d08, 0x580c);
		ar7240_reg_wr(0x18116cc0, 0x1021265e);
		ar7240_reg_wr(0x18116cc8, 0x580c);
		ar7240_reg_wr(0xb80f0008, 0x0);
		ar7240_reg_wr(0xb80600bc, 0x86000);
	}
	pci_init_board();
#else
	serial_puts("dont init PLL & DDR\n");
#endif

	switch (interface) {
	case HIF_USB:
		serial_puts("fw1: USB Init\n");
		break;
	case HIF_PCIE:
		serial_puts("fw1: PCIe Init\n");
		break;
	case HIF_GMAC:
		serial_puts("fw1: GMAC Init\n");
		gmac_fwd_main();
		break;
	case HIF_NAND:
		serial_puts("fw1: Nand Init\n");
		break;
	}

	serial_puts("leave FW1\n");
}
