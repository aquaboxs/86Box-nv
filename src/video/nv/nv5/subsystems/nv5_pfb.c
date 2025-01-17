/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PFB: Framebuffer
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

// Functions only used in this translation unit
uint32_t nv5_pfb_config0_read();
void nv5_pfb_config0_write(uint32_t val);

nv_register_t pfb_registers[] = {
    { NV5_PFB_BOOT, "PFB Boot Config", NULL, NULL},
    { NV5_PFB_CONFIG_0, "PFB Framebuffer Config 0", nv5_pfb_config0_read, nv5_pfb_config0_write },
    { NV5_PFB_CONFIG_1, "PFB Framebuffer Config 1", NULL, NULL },
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

void nv5_pfb_init()
{  
    nv_log("NV5: Initialising PFB...");

    nv5->pfb.boot = (NV5_PFB_BOOT_RAM_EXTENSION_NONE << (NV5_PFB_BOOT_RAM_EXTENSION)
    | (NV5_PFB_BOOT_RAM_DATA_TWIDDLE_OFF << NV5_PFB_BOOT_RAM_DATA_TWIDDLE)
    | (NV5_PFB_BOOT_RAM_BANKS_4 << NV5_PFB_BOOT_RAM_BANKS)
    | (NV5_PFB_BOOT_RAM_WIDTH_64 << NV5_PFB_BOOT_RAM_WIDTH)
    | (NV5_PFB_BOOT_RAM_AMOUNT_32MB << NV5_PFB_BOOT_RAM_AMOUNT)
    );

    nv_log("Done\n");
}

uint32_t nv5_pfb_read(uint32_t address) 
{ 
    nv_register_t* reg = nv_get_register(address, pfb_registers, sizeof(pfb_registers)/sizeof(pfb_registers[0]));

    uint32_t ret = 0x00;

    // todo: friendly logging

    nv_log("NV5: PFB Read from 0x%08x", address);

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
                case NV5_PFB_BOOT:
                    ret = nv5->pfb.boot;
                    break;
                // Config 0 has a read/write function
                case NV5_PFB_CONFIG_1:
                    ret = nv5->pfb.config_1;
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

void nv5_pfb_write(uint32_t address, uint32_t value) 
{
    nv_register_t* reg = nv_get_register(address, pfb_registers, sizeof(pfb_registers)/sizeof(pfb_registers[0]));

    nv_log("NV5: PFB Write 0x%08x -> 0x%08x", value, address);

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
                                // Config 0 has a read/write function
                case NV5_PFB_CONFIG_1:              // Config Register 1
                    nv5->pfb.config_1 = value;
            }
        }
    }
}

uint32_t nv5_pfb_config0_read()
{
    return nv5->pfb.config_0;
}

void nv5_pfb_config0_write(uint32_t val)
{
    nv5->pfb.config_0 = val;

    // i think the actual size and pixel depth are set in PRAMDAC
    // so we don't update things here for now

    uint32_t new_pfb_htotal = (nv5->pfb.config_0 & 0x3F) << 5;
    uint32_t new_bit_depth = (nv5->pfb.config_0 >> 8) & 0x03;
    nv_log("NV5: Framebuffer Config Change\n");
    nv_log("NV5: Horizontal Size=%d pixels\n", new_pfb_htotal); 

    if (new_bit_depth == NV5_PFB_CONFIG_0_DEPTH_8BPP)
        nv_log("NV5: Bit Depth=8bpp\n");
    else if (new_bit_depth == NV5_PFB_CONFIG_0_DEPTH_16BPP)
        nv_log("NV5: Bit Depth=16bpp\n");
    else if (new_bit_depth == NV5_PFB_CONFIG_0_DEPTH_32BPP)
        nv_log("NV5: Bit Depth=32bpp\n");
        
}