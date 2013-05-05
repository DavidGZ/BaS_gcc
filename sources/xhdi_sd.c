/*
 * xhdi_sd.c
 *
 * This file is part of BaS_gcc.
 *
 * BaS_gcc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BaS_gcc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BaS_gcc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Created on: 01.05.2013
 *  Copyright 2012 M. Fröschle
 */

#include <stdint.h>
#include <stddef.h>

#include "xhdi_sd.h"
#include "bas_printf.h"

#define DRIVER_VERSION	0x130

typedef struct
{
  uint16_t recsiz;	/* bytes per sector                    */
  uint16_t clsiz;	/* sectors per cluster                 */
  uint16_t clsizb;	/* bytes per cluster                   */
  uint16_t rdlen;	/* directory length                    */
  uint16_t fsiz;	/* length of FAT                       */
  uint16_t fatrec;	/* start of 2nd FAT                    */
  uint16_t datrec;	/* 1st free sector                     */
  uint16_t numcl;	/* overall number of clusters          */
  uint16_t bflags;	/* flags                               */
  	  	  	  	  	/* Bit 0: 0 (12-Bit-FAT), 1 16-Bit-FAT */
  	  	  	  	    /* Bit 1: 0 (two FATs), 1 (one FAT)    */
} __attribute__((__packed__)) BPB;

static BPB sd_bpb[4];	/* space for four partitions on SD card */

static xhdi_call_fun old_vector = NULL;

__attribute__((__interrupt__)) xhdi_call_fun xhdi_sd_install(xhdi_call_fun ov)
{
	old_vector = ov;

	__asm__ __volatile__ (
			"move.l		%[xhdi_call],d1\n\t"
			"move.l		d1,(sp)\n\t"	/* FIXME: dirty overwrite of saved register on stack with return value */
			: /* output */
			: [xhdi_call]"g"(xhdi_call)
			: "d1","memory");

	/*
	 * this is just to make the compiler happy, the return value is overwritten anyway. Therefore we previously overwrite
	 * the saved register value on the stack so everything is as we want it to be
	 */
	return (xhdi_call_fun) xhdi_call;
}

uint16_t xhdi_version(void)
{
	xprintf("xhdi_version() called\r\n");
	return DRIVER_VERSION;
}

uint32_t xhdi_inquire_target(uint16_t major, uint16_t minor, uint32_t *block_size, uint32_t *flags,
		char *product_name)
{
	xprintf("xhdi_inquire_target() called\r\n");
	if (block_size != NULL)
	{
		*block_size = 512;
	}
	*flags = XH_TARGET_REMOVABLE;	/* indicate that SD card can be removed */

	return E_OK;
}

uint32_t xhdi_reserve(uint16_t major, uint16_t minor, uint16_t do_reserve, uint16_t key)
{
	xprintf("xhdi_reserve() called\r\n");
	return ERROR;	/* device cannot be reserved */
}

uint32_t xhdi_lock(uint16_t major, uint16_t minor, uint16_t do_lock, uint16_t key)
{
	xprintf("xhdi_lock() called\r\n");
	return ERROR;	/* device cannot be locked */
}

uint32_t xhdi_stop(uint16_t major, uint16_t minor, uint16_t do_stop, uint16_t key)
{
	xprintf("xhdi_stop() called\r\n");
	return ERROR;	/* device cannot be locked */
}

uint32_t xhdi_eject(uint16_t major, uint16_t minor, uint16_t do_eject, uint16_t key)
{
	xprintf("xhdi_eject() called\r\n");
	return ERROR;	/* device cannot be ejected */
}

uint32_t xhdi_drivemap(void)
{
<<<<<<< .mine
	xprintf("xhdi_drivemap() called\r\n");
	long map = (1 << ('S' - 'A'));
=======
	uint32_t map = (1 << ('S' - 'A'));
>>>>>>> .r549

	return map;
}

uint32_t xhdi_inquire_device(uint16_t bios_device, uint16_t *major, uint16_t *minor,
        uint32_t *start_sector, /* BPB */ void *bpb)
{
	xprintf("xhdi_inquire_device() called\r\n");
	return ERROR;
}

uint32_t xhdi_inquire_driver(uint16_t bios_device, char *name, char *version,
		char *company, uint16_t *ahdi_version, uint16_t *maxIPL)
{
	xprintf("xhdi_inquire_driver() called\r\n");
	return ERROR;
}

uint32_t xhdi_new_cookie(uint32_t newcookie)
{
	xprintf("xhdi_new_cookie() called\r\n");
	return ERROR;
}

uint32_t xhdi_read_write(uint16_t major, uint16_t minor, uint16_t rwflag,
        uint32_t recno, uint16_t count, void *buf)
{
	xprintf("xhdi_read_write() called\r\n");
	return ERROR;
}

uint32_t xhdi_inquire_target2(uint16_t major, uint16_t minor, uint32_t *block_size,
        uint32_t *device_flags, char *product_name, uint16_t stringlen)
{
	xprintf("xhdi_inquire_target2() called\r\n");
	return ERROR;
}

uint32_t xhdi_inquire_device2(uint16_t bios_device, uint16_t *major, uint16_t *minor,
        uint32_t *start_sector, BPB *bpb, uint32_t *blocks, char *partid)
{
	xprintf("xhdi_inquire_device2() called\r\n");
	return ERROR;
}

uint32_t xhdi_driver_special(uint32_t key1, uint32_t key2, uint16_t subopcode, void *data)
{
	xprintf("xhdi_driver_special() called\r\n");
	return ERROR;
}

uint32_t xhdi_get_capacity(uint16_t major, uint16_t minor, uint32_t *blocks, uint32_t *bs)
{
	xprintf("xhdi_get_capacity() called\r\n");
	return ERROR;
}

uint32_t xhdi_medium_changed(uint16_t major, uint16_t minor)
{
	xprintf("xhdi_medium_changed() called\r\n");
	return ERROR;
}

uint32_t xhdi_mint_info(uint16_t opcode, void *data)
{
	xprintf("xhdi_mint_info() called\r\n");
	return ERROR;
}

uint32_t xhdi_dos_limits(uint16_t which, uint32_t limit)
{
	xprintf("xhdi_dos_limits() called\r\n");
	return ERROR;
}

uint32_t xhdi_last_access(uint16_t major, uint16_t minor, uint32_t *ms)
{
	xprintf("xhdi_last_access() called\r\n");
	return ERROR;
}

uint32_t xhdi_reaccess(uint16_t major, uint16_t minor)
{
	xprintf("xhdi_reaccess() called\r\n");
	return ERROR;
}