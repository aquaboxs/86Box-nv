/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PFIFO hashtable (Quickly access submitted DMA objects)
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

/* This implements the hash that all the objects are stored within.
It is used to get the offset within RAMHT of a graphics object.
 */

uint32_t nv5_ramht_hash(nv5_pramin_name_t name, uint32_t channel)
{
    uint32_t hash = (name.byte_high ^ name.byte_mid2 ^ name.byte_mid1 ^ name.byte_low ^ (uint8_t)channel);
    nv_log("NV5: Generating RAMHT hash (RAMHT slot=0x%04x (from name 0x%08x for DMA channel 0x%04x)\n)", name, channel);
    return hash;
}


uint32_t nv5_ramht_read(uint32_t address)
{
    nv_log("NV5: RAMHT (Graphics object storage hashtable) Read (0x%04x), UNIMPLEMENTED - RETURNING 0x00", address);
}

void nv5_ramht_write(uint32_t address, uint32_t value)
{
    nv_log("NV5: RAMHT (Graphics object storage hashtable) Write (0x%04x -> 0x%04x), UNIMPLEMENTED", value, address);
}
