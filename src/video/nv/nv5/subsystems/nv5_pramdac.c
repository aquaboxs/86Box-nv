/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 bringup and device emulation.
 *
 *
 *
 * Authors: aquaboxs, <aquaboxstudios87@gmail.com>
 *
 *          Copyright 2025 aquaboxs
 */

// nv5_pramdac.c: nv5 RAMDAC
// Todo: Allow overridability using 68050C register...

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

void nv5_pramdac_init()
{
    nv_log("NV5: Initialising PRAMDAC\n");

    // defaults, these come from vbios in reality
    // driver defaults are nonsensical(?), or the algorithm is wrong
    // munged this to 100mhz for now
    nv5->pramdac.memory_clock_m = nv5->pramdac.pixel_clock_m = 0x07;
    nv5->pramdac.memory_clock_n = nv5->pramdac.pixel_clock_n = 0xc8;
    nv5->pramdac.memory_clock_p = nv5->pramdac.pixel_clock_p = 0x0c;

    nv5_pramdac_set_pixel_clock();
    nv5_pramdac_set_vram_clock();

    nv_log("NV5: Initialising PRAMDAC: Done\n");
}

// Polls the pixel clock.
// This updates the 2D/3D engine PGRAPH
void nv5_pramdac_pixel_clock_poll(double real_time)
{
    // TODO: ????
}

// Polls the memory clock.
void nv5_pramdac_memory_clock_poll(double real_time)
{
    nv5_ptimer_tick(real_time);
    // TODO: UPDATE PGRAPH!
}

// Gets the vram clock register.
uint32_t nv5_pramdac_get_vram_clock_register()
{
    // the clock format is packed into 19 bits
    // M divisor            [7-0]
    // N divisor            [16-8]
    // P divisor            [18-16]
    return (nv5->pramdac.memory_clock_m)
    + (nv5->pramdac.memory_clock_n << 8)
    + (nv5->pramdac.memory_clock_p << 16); // 0-3
}

uint32_t nv5_pramdac_get_pixel_clock_register()
{
    return (nv5->pramdac.pixel_clock_m)
    + (nv5->pramdac.pixel_clock_n << 8)
    + (nv5->pramdac.pixel_clock_p << 16); // 0-3
}

void nv5_pramdac_set_vram_clock_register(uint32_t value)
{
    nv5->pramdac.memory_clock_m = value & 0xFF;
    nv5->pramdac.memory_clock_n = (value >> 8) & 0xFF;
    nv5->pramdac.memory_clock_p = (value >> 16) & 0x07;
    
    nv5_pramdac_set_vram_clock();
}

void nv5_pramdac_set_pixel_clock_register(uint32_t value)
{
    nv5->pramdac.pixel_clock_m = value & 0xFF;
    nv5->pramdac.pixel_clock_n = (value >> 8) & 0xFF;
    nv5->pramdac.pixel_clock_p = (value >> 16) & 0x07;

    nv5_pramdac_set_pixel_clock();
}

void nv5_pramdac_set_vram_clock()
{
    // from driver and vbios source
    float frequency = 13500000.0f;

    // prevent division by 0
    if (nv5->pramdac.memory_clock_m == 0)
        nv5->pramdac.memory_clock_m = 1;
    
    if (nv5->pramdac.memory_clock_n == 0)
        nv5->pramdac.memory_clock_n = 1;
    
    // Convert to microseconds
    frequency = (frequency * nv5->pramdac.memory_clock_n) / (nv5->pramdac.memory_clock_m << nv5->pramdac.memory_clock_p); 

    double time = (1000000.0 * NV5_86BOX_TIMER_SYSTEM_FIX_QUOTIENT) / (double)frequency; // needs to be a double for 86box

    nv_log("NV5: Memory clock = %.2f MHz\n", frequency / 1000000.0f);    

    nv5->nvbase.memory_clock_frequency = frequency;
    
    // Create and start if it it's not running.
    if (!nv5->nvbase.memory_clock_timer)
    {
        nv5->nvbase.memory_clock_timer = rivatimer_create(time, nv5_pramdac_memory_clock_poll);
        rivatimer_start(nv5->nvbase.memory_clock_timer);
    }

    rivatimer_set_period(nv5->nvbase.memory_clock_timer, time);
}

void nv5_pramdac_set_pixel_clock()
{
    // frequency divider algorithm from old varcem/86box/pcbox riva driver,
    // verified by reversing NT drivers v1.50e CalcMNP [symbols] function

    // todo: actually implement it

    // missing section
    // not really needed.
    // if (nv5->pfb.boot.clock_crystal == CLOCK_CRYSTAL_13500)
    // {
    //      freq = 13500000.0f;
    // }
    // else 
    //
    // {
    //      freq = 14318000.0f;
    // }

    float frequency = 13500000.0f;

    // prevent division by 0
    if (nv5->pramdac.pixel_clock_m == 0)
        nv5->pramdac.pixel_clock_m = 1;
    
    if (nv5->pramdac.memory_clock_n == 0)
        nv5->pramdac.memory_clock_n = 1;

    frequency = (frequency * nv5->pramdac.pixel_clock_n) / (nv5->pramdac.pixel_clock_m << nv5->pramdac.pixel_clock_p); 

    nv5->nvbase.svga.clock = cpuclock / frequency;

    double time = (1000000.0 * NV5_86BOX_TIMER_SYSTEM_FIX_QUOTIENT) / (double)frequency; // needs to be a double for 86box

    nv_log("NV5: Pixel clock = %.2f MHz\n", frequency / 1000000.0f);

    nv5->nvbase.pixel_clock_frequency = frequency;

    // Create and start if it it's not running.
    if (!nv5->nvbase.pixel_clock_timer)
    {
        nv5->nvbase.pixel_clock_timer = rivatimer_create(time, nv5_pramdac_pixel_clock_poll);
        rivatimer_start(nv5->nvbase.pixel_clock_timer);
    }

    rivatimer_set_period(nv5->nvbase.pixel_clock_timer, time);
}

//
// ****** PRAMDAC register list START ******
//

// NULL means handle in read functions
nv_register_t pramdac_registers[] = 
{
    { NV5_PRAMDAC_CLOCK_PIXEL, "PRAMDAC - nv5 GPU Core - Pixel clock", nv5_pramdac_get_pixel_clock_register, nv5_pramdac_set_pixel_clock_register },
    { NV5_PRAMDAC_CLOCK_MEMORY, "PRAMDAC - nv5 GPU Core - Memory clock", nv5_pramdac_get_vram_clock_register, nv5_pramdac_set_vram_clock_register },
    { NV5_PRAMDAC_COEFF_SELECT, "PRAMDAC - PLL Clock Coefficient Select", NULL, NULL},
    { NV5_PRAMDAC_GENERAL_CONTROL, "PRAMDAC - General Control", NULL, NULL },
    { NV5_PRAMDAC_VSERR_WIDTH, "PRAMDAC - Vertical Sync Error Width", NULL, NULL},
    { NV5_PRAMDAC_VEQU_END, "PRAMDAC - VEqu End", NULL, NULL},
    { NV5_PRAMDAC_VBBLANK_END, "PRAMDAC - VBBlank End", NULL, NULL},
    { NV5_PRAMDAC_VBLANK_END, "PRAMDAC - Vertical Blanking Interval End", NULL, NULL},
    { NV5_PRAMDAC_VBLANK_START, "PRAMDAC - Vertical Blanking Interval Start", NULL, NULL},
    { NV5_PRAMDAC_VEQU_START, "PRAMDAC - VEqu Start", NULL, NULL},
    { NV5_PRAMDAC_VTOTAL, "PRAMDAC - Total Vertical Lines", NULL, NULL},
    { NV5_PRAMDAC_HSYNC_WIDTH, "PRAMDAC - Horizontal Sync Pulse Width", NULL, NULL},
    { NV5_PRAMDAC_HBURST_START, "PRAMDAC - Horizontal Burst Signal Start", NULL, NULL},
    { NV5_PRAMDAC_HBURST_END, "PRAMDAC - Horizontal Burst Signal Start", NULL, NULL},
    { NV5_PRAMDAC_HTOTAL, "PRAMDAC - Total Horizontal Lines", NULL, NULL},
    { NV5_PRAMDAC_HEQU_WIDTH, "PRAMDAC - HEqu End", NULL, NULL},
    { NV5_PRAMDAC_HSERR_WIDTH, "PRAMDAC - Horizontal Sync Error", NULL, NULL},
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

//
// ****** Read/Write functions start ******
//

uint32_t nv5_pramdac_read(uint32_t address) 
{ 
    nv_register_t* reg = nv_get_register(address, pramdac_registers, sizeof(pramdac_registers)/sizeof(pramdac_registers[0]));

    uint32_t ret = 0x00;

    // todo: friendly logging

    nv_log("NV5: PRAMDAC Read from 0x%08x", address);

    // if the register actually exists
    if (reg)
    {
        // on-read function
        if (reg->on_read)
            ret = reg->on_read();
        else
        {
            //s hould be pretty easy to understand
            switch (reg->address)
            {
                case NV5_PRAMDAC_COEFF_SELECT:
                    ret = nv5->pramdac.coeff_select;
                    break;
                case NV5_PRAMDAC_GENERAL_CONTROL:
                    ret = nv5->pramdac.general_control;
                    break;
                case NV5_PRAMDAC_VSERR_WIDTH:
                    ret = nv5->pramdac.vserr_width;
                    break;
                case NV5_PRAMDAC_VBBLANK_END:
                    ret = nv5->pramdac.vbblank_end;
                    break;
                case NV5_PRAMDAC_VBLANK_END:
                    ret = nv5->pramdac.vblank_end;
                    break;
                case NV5_PRAMDAC_VBLANK_START:
                    ret = nv5->pramdac.vblank_start;
                    break;
                case NV5_PRAMDAC_VEQU_START:
                    ret = nv5->pramdac.vequ_start;
                    break;
                case NV5_PRAMDAC_VTOTAL:
                    ret = nv5->pramdac.vtotal;
                    break;
                case NV5_PRAMDAC_HSYNC_WIDTH:
                    ret = nv5->pramdac.hsync_width;
                    break;
                case NV5_PRAMDAC_HBURST_START:
                    ret = nv5->pramdac.hburst_start;
                    break;
                case NV5_PRAMDAC_HBURST_END:
                    ret = nv5->pramdac.hburst_end;
                    break;
                case NV5_PRAMDAC_HBLANK_START:
                    ret = nv5->pramdac.hblank_start;
                    break;
                case NV5_PRAMDAC_HBLANK_END:
                    ret =  nv5->pramdac.hblank_end;
                    break;
                case NV5_PRAMDAC_HTOTAL:
                    ret = nv5->pramdac.htotal;
                    break;
                case NV5_PRAMDAC_HEQU_WIDTH:
                    ret = nv5->pramdac.hequ_width;
                    break;
                case NV5_PRAMDAC_HSERR_WIDTH:
                    ret = nv5->pramdac.hserr_width;
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

void nv5_pramdac_write(uint32_t address, uint32_t value) 
{
    nv_register_t* reg = nv_get_register(address, pramdac_registers, sizeof(pramdac_registers)/sizeof(pramdac_registers[0]));

    nv_log("NV5: PRAMDAC Write 0x%08x -> 0x%08x", value, address);

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
            //s hould be pretty easy to understand
            // we also update the SVGA state here
            switch (reg->address)
            {
                case NV5_PRAMDAC_COEFF_SELECT:
                    nv5->pramdac.coeff_select = value;
                    break;
                case NV5_PRAMDAC_GENERAL_CONTROL:
                    nv5->pramdac.general_control = value;
                    break;
                case NV5_PRAMDAC_VSERR_WIDTH:
                    //vslines?
                    nv5->pramdac.vserr_width = value;
                    break;
                case NV5_PRAMDAC_VBBLANK_END:
                    nv5->pramdac.vbblank_end = value;
                    break;
                case NV5_PRAMDAC_VBLANK_END:
                    nv5->pramdac.vblank_end = value;
                    break;
                case NV5_PRAMDAC_VBLANK_START:
                    nv5->nvbase.svga.vblankstart = value;
                    nv5->pramdac.vblank_start = value;
                    break;
                case NV5_PRAMDAC_VEQU_START:
                    nv5->pramdac.vequ_start = value;
                    break;
                case NV5_PRAMDAC_VTOTAL:
                    nv5->pramdac.vtotal = value;
                    nv5->nvbase.svga.vtotal = value;
                    break;
                case NV5_PRAMDAC_HSYNC_WIDTH:
                    nv5->pramdac.hsync_width = value;
                    break;
                case NV5_PRAMDAC_HBURST_START:
                    nv5->pramdac.hburst_start = value;
                    break;
                case NV5_PRAMDAC_HBURST_END:
                    nv5->pramdac.hburst_end = value;
                    break;
                case NV5_PRAMDAC_HBLANK_START:
                    nv5->nvbase.svga.hblankstart = value;
                    nv5->pramdac.hblank_start = value;
                    break;
                case NV5_PRAMDAC_HBLANK_END:
                    nv5->nvbase.svga.hblank_end_val = value;
                    nv5->pramdac.hblank_end = value;
                    break;
                case NV5_PRAMDAC_HTOTAL:
                    nv5->pramdac.htotal = value;
                    nv5->nvbase.svga.htotal = value;
                    break;
                case NV5_PRAMDAC_HEQU_WIDTH:
                    nv5->pramdac.hequ_width = value;
                    break;
                case NV5_PRAMDAC_HSERR_WIDTH:
                    nv5->pramdac.hserr_width = value;
                    break;
            }
        }
    }
}