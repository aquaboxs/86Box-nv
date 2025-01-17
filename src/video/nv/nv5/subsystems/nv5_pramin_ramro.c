/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PFIFO ram runout area (you fucked up)
 *
 *
 *
 * Authors: aquaboxs, <aquaboxstudios87@gmail.com>
 *
 *          Copyright 2025 aquaboxs
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <86Box/86box.h>
#include <86Box/device.h>
#include <86Box/mem.h>
#include <86box/pci.h>
#include <86Box/rom.h> // DEPENDENT!!!
#include <86Box/video.h>
#include <86Box/nv/vid_nv.h>
#include <86Box/nv/vid_nv5.h>

uint32_t nv5_ramro_read(uint32_t address)
{
    nv_log("NV5: RAM Runout (invalid dma object submission) Read (0x%04x), UNIMPLEMENTED - RETURNING 0x00", address);
}

void nv5_ramro_write(uint32_t address, uint32_t value)
{
    nv_log("NV5: RAM Runout WRITE, OH CRAP!!!! (0x%04x -> 0x%04x), UNIMPLEMENTED\n (Todo: Read the entries...)", value, address);
}