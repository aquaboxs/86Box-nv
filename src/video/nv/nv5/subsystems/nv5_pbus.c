/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PBUS: 128-bit unified bus
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

// nv5 PBUS RMA - Real Mode Access for VBIOS
// This basically works like a shifter, you write one byte at a time from [0x3d0...0x3d3] and it shifts it in to build a 32-bit MMIO address...
// Putting this in pbus because imo it makes the most sense (related to memory access/memory interface)

nv_register_t pbus_registers[] = {
    { NV5_PBUS_INTR, "PBUS - Interrupt Status", NULL, NULL},
    { NV5_PBUS_INTR_EN, "PBUS - Interrupt Enable", NULL, NULL,},
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

void nv5_pbus_init()
{
    nv_log("NV5: Initialising PBUS...");

    nv_log("Done\n");    
}

uint32_t nv5_pbus_read(uint32_t address) 
{ 
    nv_register_t* reg = nv_get_register(address, pbus_registers, sizeof(pbus_registers)/sizeof(pbus_registers[0]));

    uint32_t ret = 0x00; 

    // todo: friendly logging
    
    nv_log("NV5: PBUS Read from 0x%08x", address);

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
                case NV5_PBUS_INTR:
                    ret = nv5->pbus.interrupt_status;
                    break;
                case NV5_PBUS_INTR_EN:
                    ret = nv5->pbus.interrupt_enable;
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

void nv5_pbus_write(uint32_t address, uint32_t value) 
{
    nv_register_t* reg = nv_get_register(address, pbus_registers, sizeof(pbus_registers)/sizeof(pbus_registers[0]));

    nv_log("NV5: PBUS Write 0x%08x -> 0x%08x\n", value, address);

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
                // Interrupt registers
                // Interrupt state:
                // Bit 0 - PCI Bus Error
                case NV5_PBUS_INTR:
                    nv5->pbus.interrupt_status &= ~value;
                    nv5_pmc_clear_interrupts();
                    break;
                case NV5_PBUS_INTR_EN:
                    nv5->pbus.interrupt_enable = value & 0x00000001;
                    break;
            }

        }
    }
}

uint8_t nv5_pbus_rma_read(uint16_t addr)
{
    addr &= 0xFF;
    uint32_t real_final_address;
    uint8_t ret;

    switch (addr)
    {
        // signature so you know reads work
        case 0x00:
            ret = NV5_RMA_SIGNATURE_MSB;
            break;
        case 0x01:
            ret = NV5_RMA_SIGNATURE_BYTE2;
            break;
        case 0x02:
            ret = NV5_RMA_SIGNATURE_BYTE1;
            break;       
        case 0x03:
            ret = NV5_RMA_SIGNATURE_LSB;
            break;
        case 0x08 ... 0x0B:
            // reads must be dword aligned
            real_final_address = (nv5->pbus.rma.addr + (addr & 0x03));

            if (nv5->pbus.rma.addr < NV5_MMIO_SIZE) 
                ret = nv5_mmio_read8(real_final_address, NULL);
            else 
            {
                // ABOVE CODE IS TEMPORARY UNTIL PNVM EXISTS!!!!!
                // would svga->fast work?
                nv5->nvbase.svga.chain4 = true;
                nv5->nvbase.svga.packed_chain4 = true;
                ret = svga_read_linear((real_final_address - NV5_MMIO_SIZE) & (nv5->nvbase.svga.vram_max - 1), &nv5->nvbase.svga);
                nv5->nvbase.svga.chain4 = false;
                nv5->nvbase.svga.packed_chain4 = false;
            }

            // log current location for vbios RE
            nv_log("NV5: MMIO Real Mode Access read, initial address=0x%04x final RMA MMIO address=0x%08x data=0x%08x\n",
                addr, real_final_address, ret);

            break;
    }

    return ret; 
}

// Implements a 32-bit write using 16 bit port number
void nv5_pbus_rma_write(uint16_t addr, uint8_t val)
{
    // addresses are in reality 8bit so just mask it to be safe
    addr &= 0xFF;

    // format:
    // 0x00     ID
    // 0x04     Pointer to data
    // 0x08     Data port(?) 
    // 0x0B     Data - 32bit. SENT IN THE RIGHT ORDER FOR ONCE WAHOO!
    // 0x10     Increment (?) data - implemented the same as data for now 

    if (addr < 0x08)
    {
        switch (addr % 0x04)
        {
            case 0x00: // lowest byte
                nv5->pbus.rma.addr &= ~0xff;
                nv5->pbus.rma.addr |= val;
                break;
            case 0x01: // 2nd highest byte
                nv5->pbus.rma.addr &= ~0xff00;
                nv5->pbus.rma.addr |= (val << 8);
                break;
            case 0x02: // 3rd highest byte
                nv5->pbus.rma.addr &= ~0xff0000;
                nv5->pbus.rma.addr |= (val << 16);
                break;
            case 0x03: // 4th highest byte 
                nv5->pbus.rma.addr &= ~0xff000000;
                nv5->pbus.rma.addr |= (val << 24);
                break;
        }
    }
    // Data to send to MMIO
    else
    {
        switch (addr % 0x04)
        {
            case 0x00: // lowest byte
                nv5->pbus.rma.data &= ~0xff;
                nv5->pbus.rma.data |= val;
                break;
            case 0x01: // 2nd highest byte
                nv5->pbus.rma.data &= ~0xff00;
                nv5->pbus.rma.data |= (val << 8);
                break;
            case 0x02: // 3rd highest byte
                nv5->pbus.rma.data &= ~0xff0000;
                nv5->pbus.rma.data |= (val << 16);
                break;
            case 0x03: // 4th highest byte 
                nv5->pbus.rma.data &= ~0xff000000;
                nv5->pbus.rma.data |= (val << 24);

                nv_log("NV5: MMIO Real Mode Access write transaction complete, initial address=0x%04x final RMA MMIO address=0x%08x data=0x%08x\n",
                addr, nv5->pbus.rma.addr, nv5->pbus.rma.data);

                if (nv5->pbus.rma.addr < NV5_MMIO_SIZE) 
                    nv5_mmio_write32(nv5->pbus.rma.addr, nv5->pbus.rma.data, NULL);
                else // failsafe code, i don't think you will ever write outside of VRAM?
                {
                    nv5->nvbase.svga.chain4 = true;
                    nv5->nvbase.svga.packed_chain4 = true;
                    svga_writel_linear((nv5->pbus.rma.addr - NV5_MMIO_SIZE) & (nv5->nvbase.svga.vram_max - 1), nv5->pbus.rma.data, &nv5->nvbase.svga);
                    nv5->nvbase.svga.chain4 = false;
                    nv5->nvbase.svga.packed_chain4 = false;
                }
                    
                    
                break;
        }
    }

    if (addr & 0x10)
        nv5->pbus.rma.addr += 0x04; // Alignment
}