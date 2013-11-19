#include "mmu.h"

#define ACR_BA(x)                   ((x) & 0xffff0000)
#define ACR_ADMSK(x)                (((x) & 0xffff) << 16)
#define ACR_E(x)                    (((x) & 1) << 15)

#define ACR_S(x)                    (((x) & 3) << 13)
#define ACR_S_USERMODE              0
#define ACR_S_SUPERVISOR_MODE       1
#define ACR_S_ALL                   2

#define ACR_AMM(x)                  (((x) & 1) << 10)

#define ACR_CM(x)                   (((x) & 3) << 5)
#define ACR_CM_CACHEABLE_WT         0x0
#define ACR_CM_CACHEABLE_CB         0x1
#define ACR_CM_CACHE_INH_PRECISE    0x2
#define ACR_CM_CACHE_INH_IMPRECISE  0x3

#define ACR_SP(x)           (((x) & 1) << 3)
#define ACR_W(x)            (((x) & 1) << 2)

#include <stdint.h>
#include "bas_printf.h"
#include "bas_types.h"
#include "MCF5475.h"
#include "pci.h"
#include "cache.h"
#if MACHINE_FIREBEE
#include "firebee.h"
#elif MACHINE_M5484LITE
#include "m5484l.h"
#endif /* MACHINE_FIREBEE */
			
/*
 * set ASID register
 * saves new value to rt_asid and returns former value
 */
inline uint32_t set_asid(uint32_t value)
{
	extern long rt_asid;
	uint32_t ret = rt_asid;

	__asm__ __volatile__(
		"movec		%[value],ASID\n\t"
		: /* no output */
		: [value] "r" (value)
		: 
	);

	rt_asid = value;

	return ret;
}

							
/*
 * set ACRx register
 * saves new value to rt_acrx and returns former value
 */
inline uint32_t set_acr0(uint32_t value)
{
	extern uint32_t rt_acr0;
	uint32_t ret = rt_acr0;
	
	__asm__ __volatile__(
		"movec		%[value],ACR0\n\t"
		: /* not output */
		: [value] "r" (value)
		:
	);
	rt_acr0 = value;

	return ret;
}

/*
 * set ACRx register
 * saves new value to rt_acrx and returns former value
 */
inline uint32_t set_acr1(uint32_t value)
{
	extern uint32_t rt_acr1;
	uint32_t ret = rt_acr1;
	
	__asm__ __volatile__(
		"movec		%[value],ACR1\n\t"
		: /* not output */
		: [value] "r" (value)
		:
	);
	rt_acr1 = value;

	return ret;
}


/*
 * set ACRx register
 * saves new value to rt_acrx and returns former value
 */
inline uint32_t set_acr2(uint32_t value)
{
	extern uint32_t rt_acr2;
	uint32_t ret = rt_acr2;
	
	__asm__ __volatile__(
		"movec		%[value],ACR2\n\t"
		: /* not output */
		: [value] "r" (value)
		:
	);
	rt_acr2 = value;

	return ret;
}

/*
 * set ACRx register
 * saves new value to rt_acrx and returns former value
 */
inline uint32_t set_acr3(uint32_t value)
{
	extern uint32_t rt_acr3;
	uint32_t ret = rt_acr3;
	
	__asm__ __volatile__(
		"movec		%[value],ACR3\n\t"
		: /* not output */
		: [value] "r" (value)
		:
	);
	rt_acr3 = value;

	return ret;
}

inline uint32_t set_mmubar(uint32_t value)
{
	extern uint32_t rt_mmubar;
	uint32_t ret = rt_mmubar;

	__asm__ __volatile__(
		"movec		%[value],MMUBAR\n\t"
		: /* no output */
		: [value] "r" (value)
		: /* no clobber */
	);
	rt_mmubar = value;

	return ret;
}

void mmu_init(void)
{
	extern uint8_t _MMUBAR[];
	uint32_t MMUBAR = (uint32_t) &_MMUBAR[0];
	extern uint8_t _TOS[];
	uint32_t TOS = (uint32_t) &_TOS[0];
	
	set_asid(0);			/* do not use address extension (ASID provides virtual 48 bit addresses */
	set_acr0(ACR_W(0) |								/* read and write accesses permitted */
			ACR_SP(0) |								/* supervisor and user mode access permitted */
			ACR_CM(ACR_CM_CACHE_INH_PRECISE) |		/* cache inhibit, precise */
			ACR_AMM(0) |							/* control region > 16 MB */
			ACR_S(ACR_S_ALL) |						/* match addresses in user and supervisor mode */
			ACR_E(1) |								/* enable ACR */
			ACR_ADMSK(0x3f) |						/* cover 1GB area from 0xc0000000 to 0xffffffff */
			ACR_BA(0xc0000000));					/* (equals area from 3 to 4 GB */
	
	set_acr1(0x601fc000);
	set_acr2(0xe007c400);
	set_acr3(0x0);
	set_mmubar(MMUBAR | 1);		/* set and enable MMUBAR */

	/* clear all MMU TLB entries */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_CA;

	/* create locked TLB entries */

	/*
	 * 0x0000'0000 - 0x000F'FFFF (first MB of physical memory) locked virtual = physical
	 */
	MCF_MMU_MMUTR = 0x0 | 					/* virtual address */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = 0x0 |					/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x1) |	/* cacheable, copyback */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X | 		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */

#if MACHINE_FIREBEE		/* map FPGA video memory for FireBee only */
	/*
	 * 0x00d0'0000 - 0x00df'ffff (last megabyte of ST RAM = Falcon video memory) locked ID = 6
	 * mapped to physical address 0x60d0'0000 (FPGA video memory)
	 * video RAM: read write execute normal write true
	 */
	
	MCF_MMU_MMUTR = 0x00d00000 |			/* virtual address */
					MCF_MMU_MMUTR_ID(SCA_PAGE_ID) |
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = 0x60d00000 |			/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x0) |	/* cachable writethrough */
					MCF_MMU_MMUDR_SP |		/* supervisor protect */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X |		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */

	video_tlb = 0x2000;						/* set page as video page */
	video_sbt = 0x0;						/* clear time */
#endif /* MACHINE_FIREBEE */

	/*
	 * Make the TOS (in SDRAM) read-only
	 * This maps virtual 0x00e0'0000 - 0x00ef'ffff to the same virtual address
	 */
	MCF_MMU_MMUTR = TOS |					/* virtual address */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = TOS |					/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x1) |	/* cachable copyback */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable (FIXME: for now) */
					MCF_MMU_MMUDR_X |		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */

#if MACHINE_FIREBEE
	/*
	 * Map FireBee I/O area (0xfff0'0000 - 0xffff'0000 physical) to the Falcon-compatible I/O
	 * area (0x00f0'0000 - 0x00ff'0000 virtual) for the FireBee
	 */

	MCF_MMU_MMUTR = 0x00f00000 |			/* virtual address */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = 0xfff00000 |			/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x2) |	/* nocache precise */
					MCF_MMU_MMUDR_SP |		/* supervisor protect */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X |		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */
#endif /* MACHINE_FIREBEE */

	/*
	 * Map PCI memory address space. Uncached, precise, virtual = physical.
	 * FIXME: this currently only maps the first megabyte, while in reality PCI address space should
	 * cover 128 MByte. We need to do that as special case in the MMU TLB miss exception routine
	 */

	MCF_MMU_MMUTR = PCI_MEMORY_OFFSET |		/* virtual address */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = PCI_MEMORY_OFFSET |		/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x2) |	/* nocache precise */
					MCF_MMU_MMUDR_SP |		/* supervisor protect */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X |		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */

	/*
	 * Map (locked) the last MB of physical SDRAM (this is where BaS .data and .bss reside) to the same
	 * virtual address. This is also used when BaS is in RAM
	 */

	MCF_MMU_MMUTR = SDRAM_START + SDRAM_SIZE - 0x00100000 |	/* virtual address */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = SDRAM_START + SDRAM_SIZE - 0x00100000 |	/* physical address */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x0) |	/* cacheable writethrough */
					MCF_MMU_MMUDR_SP |		/* supervisor protect */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X |		/* execute access enable */
					MCF_MMU_MMUDR_LK;		/* lock entry */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */
}

__attribute__((interrupt)) mmutr_miss()
{
	register uint32_t address asm("d0");

	xprintf("MMU TLB MISS at %p\r\n", address);
	flush_and_invalidate_caches();

	/* add missed page to TLB */
	MCF_MMU_MMUTR = (address & 0xfff00000) | /* virtual aligned to 1M */
					MCF_MMU_MMUTR_SG |		/* shared global */
					MCF_MMU_MMUTR_V;		/* valid */
	MCF_MMU_MMUDR = (address & 0xfff00000) |	/* physical aligned to 1M */
					MCF_MMU_MMUDR_SZ(0) |	/* 1 MB page size */
					MCF_MMU_MMUDR_CM(0x0) |	/* cacheable writethrough */
					MCF_MMU_MMUDR_R |		/* read access enable */
					MCF_MMU_MMUDR_W |		/* write access enable */
					MCF_MMU_MMUDR_X;		/* execute access enable */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ACC |		/* access TLB, data */
					MCF_MMU_MMUOR_UAA;		/* update allocation address field */
	MCF_MMU_MMUOR = MCF_MMU_MMUOR_ITLB | 	/* instruction */
					MCF_MMU_MMUOR_ACC |     /* access TLB */
					MCF_MMU_MMUOR_UAA;      /* update allocation address field */
}



