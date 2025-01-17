/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 pme: Nvidia Mediaport - External MPEG Decode Interface
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

nv_register_t pme_registers[] = {
    { NV5_PME_INTR, "PME - Interrupt Status", NULL, NULL},
    { NV5_PME_INTR_EN, "PME - Interrupt Enable", NULL, NULL,},
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

void nv5_pme_init()
{  
    nv_log("NV5: Initialising PME...");

    nv_log("Done\n");
}

uint32_t nv5_pme_read(uint32_t address) 
{ 
    nv_register_t* reg = nv_get_register(address, pme_registers, sizeof(pme_registers)/sizeof(pme_registers[0]));

    uint32_t ret = 0x00;

    // todo: friendly logging

    nv_log("NV5: PME Read from 0x%08x", address);

    // if the register actually exists
    if (reg)
    {
        // on-read function
        if (reg->on_read)
            ret = reg->on_read();
        else
        {   
            // Interrupt state:
            // Bit 0 - Image Notifier
            // Bit 4 - Vertical Blank Interfal Notifier
            // Bit 8 - Video Notifier
            // Bit 12 - Audio Notifier
            // Bit 16 - VMI Notifer
            switch (reg->address)
            {
                case NV5_PME_INTR:
                    ret = nv5->pme.interrupt_status;
                    break;
                case NV5_PME_INTR_EN:
                    ret = nv5->pme.interrupt_enable;
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

void nv5_pme_write(uint32_t address, uint32_t value) 
{
    nv_register_t* reg = nv_get_register(address, pme_registers, sizeof(pme_registers)/sizeof(pme_registers[0]));

    nv_log("NV5: PME Write 0x%08x -> 0x%08x\n", value, address);

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
        else
        {
            switch (reg->address)
            {
                // Interrupt state:
                // Bit 0 - Image Notifier
                // Bit 4 - Vertical Blank Interfal Notifier
                // Bit 8 - Video Notifier
                // Bit 12 - Audio Notifier
                // Bit 16 - VMI Notifer

                case NV5_PME_INTR:
                    nv5->pme.interrupt_status &= ~value;
                    nv5_pmc_clear_interrupts();
                    break;
                case NV5_PME_INTR_EN:
                    nv5->pme.interrupt_enable = value & 0x00001111;
                    break;
            }
        }

    }
    }