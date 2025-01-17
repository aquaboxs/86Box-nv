/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 pfifo (FIFO for graphics object submission)
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



//
// ****** pfifo register list START ******
//

nv_register_t pfifo_registers[] = {
    { NV5_PFIFO_INTR, "PFIFO - Interrupt Status", NULL, NULL},
    { NV5_PFIFO_INTR_EN, "PFIFO - Interrupt Enable", NULL, NULL,},
    { NV5_PFIFO_CONFIG_RAMFC, "PFIFO - RAMIN RAMFC Config", NULL, NULL },
    { NV5_PFIFO_CONFIG_RAMHT, "PFIFO - RAMIN RAMHT Config", NULL, NULL },
    { NV5_PFIFO_CONFIG_RAMRO, "PFIFO - RAMIN RAMRO Config", NULL, NULL },
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

// PFIFO init code
void nv5_pfifo_init()
{
    nv_log("NV5: Initialising PFIFO...");

    nv_log("Done!\n");    
}

uint32_t nv5_pfifo_read(uint32_t address) 
{ 
    // before doing anything, check the subsystem enablement state

    if (!(nv5->pmc.enable >> NV5_PMC_ENABLE_PFIFO)
    & NV5_PMC_ENABLE_PFIFO_ENABLED)
    {
        nv_log("NV5: Repressing PFIFO read. The subsystem is disabled according to pmc_enable, returning 0\n");
        return 0x00;
    }

    uint32_t ret = 0x00;

    nv_register_t* reg = nv_get_register(address, pfifo_registers, sizeof(pfifo_registers)/sizeof(pfifo_registers[0]));

    // todo: friendly logging
    
    nv_log("NV5: PFIFO Read from 0x%08x", address);

    // if the register actually exists
    if (reg)
    {

        // on-read function
        if (reg->on_read)
            ret = reg->on_read();
        else
        {   
            // Interrupt state:
            // Bit 0 - Cache Error
            // Bit 4 - RAMRO Triggered
            // Bit 8 - RAMRO Overflow (too many invalid dma objects)
            // Bit 12 - DMA Pusher 
            // Bit 16 - DMA Page Table Entry (pagefault?)
            switch (reg->address)
            {
                case NV5_PFIFO_INTR:
                    ret = nv5->pfifo.interrupt_status;
                    break;
                case NV5_PFIFO_INTR_EN:
                    ret = nv5->pfifo.interrupt_enable;
                    break;
                // These may need to become functions.
                case NV5_PFIFO_CONFIG_RAMFC:
                    ret = nv5->pfifo.ramfc_config;
                    break;
                case NV5_PFIFO_CONFIG_RAMHT:
                    ret = nv5->pfifo.ramht_config;
                    break;
                case NV5_PFIFO_CONFIG_RAMRO:
                    ret = nv5->pfifo.ramro_config;
                    break;
            }
        }

        if (reg->friendly_name)
            nv_log(": %s\n", reg->friendly_name);
        else   
            nv_log("\n");
    }
    else
    {
        nv_log(": Unknown register read (address=0x%08x), returning 0x00\n", address);
    }

    return ret; 
}

void nv5_pfifo_write(uint32_t address, uint32_t value) 
{
    // before doing anything, check the subsystem enablement

    if (!(nv5->pmc.enable >> NV5_PMC_ENABLE_PFIFO)
    & NV5_PMC_ENABLE_PFIFO_ENABLED)
    {
        nv_log("NV5: Repressing PFIFO write. The subsystem is disabled according to pmc_enable\n");
        return;
    }

    nv_register_t* reg = nv_get_register(address, pfifo_registers, sizeof(pfifo_registers)/sizeof(pfifo_registers[0]));

    nv_log("NV5: PFIFO Write 0x%08x -> 0x%08x\n", value, address);

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
                // Bit 0 - Cache Error
                // Bit 4 - RAMRO Triggered
                // Bit 8 - RAMRO Overflow (too many invalid dma objects)
                // Bit 12 - DMA Pusher 
                // Bit 16 - DMA Page Table Entry (pagefault?)
                case NV5_PFIFO_INTR:
                    nv5->pfifo.interrupt_status &= ~value;
                    nv5_pmc_clear_interrupts();
                    break;
                case NV5_PFIFO_INTR_EN:
                    nv5->pbus.interrupt_enable = value & 0x00001111;
                    break;
                case NV5_PFIFO_CONFIG_RAMHT:
                    nv5->pfifo.ramht_config = value;
// This code sucks a bit fix it later
#ifdef ENABLE_NV_LOG
                    uint32_t new_size_ramht = ((value >> 16) & 0x03);

                    if (new_size_ramht == 0)
                        new_size_ramht = 0x1000;
                    else if (new_size_ramht == 1)
                        new_size_ramht = 0x2000;
                    else if (new_size_ramht == 2)
                        new_size_ramht = 0x4000;
                    else if (new_size_ramht == 3)
                        new_size_ramht = 0x8000;  

                    nv_log("NV5: RAMHT Reconfiguration\n"
                    "Base Address in RAMIN: %d\n"
                    "Size: 0x%08x bytes\n", ((nv5->pfifo.ramht_config >> NV5_PFIFO_CONFIG_RAMHT_BASE_ADDRESS) & 0x0F) << 12, new_size_ramht); 
#endif
                    break;
                case NV5_PFIFO_CONFIG_RAMFC:
                    nv5->pfifo.ramfc_config = value;

                    nv_log("NV5: RAMFC Reconfiguration\n"
                    "Base Address in RAMIN: %d\n", ((nv5->pfifo.ramfc_config >> NV5_PFIFO_CONFIG_RAMFC_BASE_ADDRESS) & 0x7F) << 9); 
                    break;
                case NV5_PFIFO_CONFIG_RAMRO:
                    nv5->pfifo.ramro_config = value;

                    uint32_t new_size_ramro = ((value >> 16) & 0x01);

                    if (new_size_ramro == 0)
                        new_size_ramro = 0x200;
                    else if (new_size_ramro == 1)
                        new_size_ramro = 0x2000;
                    
                    nv_log("NV5: RAMRO Reconfiguration\n"
                    "Base Address in RAMIN: %d\n"
                    "Size: 0x%08x bytes\n", ((nv5->pfifo.ramro_config >> NV5_PFIFO_CONFIG_RAMRO_BASE_ADDRESS) & 0x7F) << 9, new_size_ramro); 
                    break;
            }
        }
    }

}