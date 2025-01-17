/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PEXTDEV - External Devices
 *                        Including straps
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

void nv5_pextdev_init()
{
    nv_log("NV5: Initialising PEXTDEV....\n");

    // Set the chip straps
    // Make these configurable in the future...

    // Current settings
    // AGP2X            Disabled
    // TV Mode          NTSC
    // Crystal          13.5 Mhz
    // Bus width        128-Bit (some gpus were sold as 64bit for cost reduction)
    // 

    nv_log("NV5: Initialising straps...\n");

    nv5->pextdev.straps =
    (NV5_PSTRAPS_AGP2X_DISABLED << NV5_PSTRAPS_AGP2X) |
    (NV5_PSTRAPS_TVMODE_NTSC << NV5_PSTRAPS_TVMODE) |
    (NV5_PSTRAPS_CRYSTAL_13500K << NV5_PSTRAPS_CRYSTAL);

    // figure out the bus
    if (nv5->nvbase.bus_generation == nv_bus_pci)
        nv5->pextdev.straps |= (NV5_PSTRAPS_BUS_TYPE_PCI << NV5_PSTRAPS_BUS_TYPE);
    else
        nv5->pextdev.straps |= (NV5_PSTRAPS_BUS_TYPE_AGP << NV5_PSTRAPS_BUS_TYPE);

    // now the lower bits 
    nv5->pextdev.straps |=
    (NV5_PSTRAPS_BUS_WIDTH_128BIT << NV5_PSTRAPS_BUS_WIDTH) |
    (NV5_PSTRAPS_BIOS_PRESENT << NV5_PSTRAPS_BIOS) |
    (NV5_PSTRAPS_BUS_SPEED_66MHZ << NV5_PSTRAPS_BUS_SPEED);

    nv_log("NV5: Straps = 0x%04x\n", nv5->pextdev.straps);
    nv_log("NV5: Initialising PEXTDEV: Done\n");
}

//
// ****** PEXTDEV register list START ******
//

nv_register_t pextdev_registers[] = {
    { NV5_PSTRAPS, "Straps - Chip Configuration", NULL, NULL },
    { NV_REG_LIST_END, NULL, NULL, NULL }, // sentinel value 
};


//
// ****** Read/Write functions start ******
//

uint32_t nv5_pextdev_read(uint32_t address) 
{ 
    nv_register_t* reg = nv_get_register(address, pextdev_registers, sizeof(pextdev_registers)/sizeof(pextdev_registers[0]));

    uint32_t ret = 0x00;

    // special consideration for straps
    if (address == NV5_PSTRAPS)
    {
        nv_log("NV5: Straps Read (current value=0x%08x)\n", nv5->pextdev.straps);
    }
    else
    {
        nv_log("NV5: PEXTDEV Read from 0x%08x", address);
    }
    
    // if the register actually exists
    if (reg)
    {
        // on-read function
        if (reg->on_read)
            ret = reg->on_read();
        else
        {
            switch (reg->address)
            {
                case NV5_PSTRAPS:
                    ret = nv5->pextdev.straps;
                    break;
            }
        }

        if (reg->friendly_name)
            nv_log(": %s (value = 0x%08x)\n", reg->friendly_name, ret);
        else   
            nv_log("\n");
    }
    else
    {
        nv_log(": Unknown register read (address=0x%08x), returning 0x00\n", address);
    }

    return ret; 
}

void nv5_pextdev_write(uint32_t address, uint32_t value) 
{
    nv_register_t* reg = nv_get_register(address, pextdev_registers, sizeof(pextdev_registers)/sizeof(pextdev_registers[0]));

    nv_log("NV5: PEXTDEV Write 0x%08x -> 0x%08x\n", value, address);

    // special consideration for straps
    if (address == NV5_PSTRAPS)
    {
        nv_log("NV5: Huh? Tried to write to the straps. Something is wrong...\n", nv5->pextdev.straps);
        return;
    }

    // if the register actually exists
    if (reg)
    {
        if (reg->friendly_name)
            nv_log(": %s\n", reg->friendly_name);
        else   
            nv_log("\n");

        // on-read function
        if (reg->on_write)
            reg->on_write(value);
    }
}