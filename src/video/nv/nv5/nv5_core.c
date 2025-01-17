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
 * Authors: aquaboxs <aquaboxstudios87@gmail.com>
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
#include <86box/io.h>
#include <86box/pci.h>
#include <86Box/rom.h> // DEPENDENT!!!
#include <86Box/video.h>
#include <86Box/nv/vid_nv.h>
#include <86Box/nv/vid_nv5.h>

nv5_t* nv5;

// Prototypes for functions only used in this translation unit
void nv5_init_mappings_mmio();
void nv5_init_mappings_svga();
bool nv5_is_svga_redirect_address(uint32_t addr);

uint8_t nv5_svga_in(uint16_t addr, void* priv);
void nv5_svga_out(uint16_t addr, uint8_t val, void* priv);

// Determine if this address needs to be redirected to the SVGA subsystem.

bool nv5_is_svga_redirect_address(uint32_t addr)
{
    return (addr >= NV5_PRMVIO_START
    && addr <= NV5_PRMVIO_END
    || addr == NV5_PRMCIO_CRTC_REGISTER_CUR_COLOR
    || addr == NV5_PRMCIO_CRTC_REGISTER_CUR_INDEX_COLOR
    || addr == NV5_PRMCIO_CRTC_REGISTER_CUR_MONO
    || addr == NV5_PRMCIO_CRTC_REGISTER_CUR_INDEX_MONO);
}

// All MMIO regs are 32-bit i believe internally
// so we have to do some munging to get this to read

// Read 8-bit MMIO
uint8_t nv5_mmio_read8(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = nv5_svga_in(real_address, nv5);

        nv_log("NV5: Redirected MMIO read8 to SVGA: addr=0x%04x returned 0x%02x\n", addr, ret);

        return ret; 
    }

    // see if unaligned reads are a problem
    ret = nv5_mmio_read32(addr, priv);
    return (uint8_t)(ret >> ((addr & 3) << 3) & 0xFF);
}

// Read 16-bit MMIO
uint16_t nv5_mmio_read16(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = nv5_svga_in(real_address, nv5)
        | (nv5_svga_in(real_address + 1, nv5) << 8);
        
        nv_log("NV5: Redirected MMIO read16 to SVGA: addr=0x%04x returned 0x%04x\n", addr, ret);

        return ret; 
    }

    ret = nv5_mmio_read32(addr, priv);
    return (uint8_t)(ret >> ((addr & 3) << 3) & 0xFFFF);
}

// Read 32-bit MMIO
uint32_t nv5_mmio_read32(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = nv5_svga_in(real_address, nv5)
        | (nv5_svga_in(real_address + 1, nv5) << 8)
        | (nv5_svga_in(real_address + 2, nv5) << 16)
        | (nv5_svga_in(real_address + 3, nv5) << 24);

        nv_log("NV5: Redirected MMIO read32 to SVGA: addr=0x%04x returned 0x%08x\n", addr, ret);

        return ret; 
    }


    return nv5_mmio_arbitrate_read(addr);
}

// Write 8-bit MMIO
void nv5_mmio_write8(uint32_t addr, uint8_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // This is weitek vga stuff
    // If we need to add more of these we can convert these to a switch statement
    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        nv_log("NV5: Redirected MMIO write8 to SVGA: addr=0x%04x val=0x%02x\n", addr, val);
        nv5_svga_out(real_address, val & 0xFF, nv5);

        return; 
    }
    
    // overwrite first 8bits of a 32 bit value
    uint32_t new_val = nv5_mmio_read32(addr, NULL);

    new_val &= (~0xFF << (addr & 3) << 3);
    new_val |= (val << ((addr & 3) << 3));

    nv5_mmio_write32(addr, new_val, priv);
}

// Write 16-bit MMIO
void nv5_mmio_write16(uint32_t addr, uint16_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // This is weitek vga stuff
    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        nv_log("NV5: Redirected MMIO write16 to SVGA: addr=0x%04x val=0x%04x\n", addr, val);
        nv5_svga_out(real_address, val & 0xFF, nv5);
        nv5_svga_out(real_address + 1, (val >> 8) & 0xFF, nv5);
        
        return; 
    }

    // overwrite first 16bits of a 32 bit value
    uint32_t new_val = nv5_mmio_read32(addr, NULL);

    new_val &= (~0xFFFF << (addr & 3) << 3);
    new_val |= (val << ((addr & 3) << 3));

    nv5_mmio_write32(addr, new_val, priv);
}

// Write 32-bit MMIO
void nv5_mmio_write32(uint32_t addr, uint32_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // This is weitek vga stuff
    if (nv5_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        nv_log("NV5: Redirected MMIO write32 to SVGA: addr=0x%04x val=0x%08x\n", addr, val);

        nv5_svga_out(real_address, val & 0xFF, nv5);
        nv5_svga_out(real_address + 1, (val >> 8) & 0xFF, nv5);
        nv5_svga_out(real_address + 2, (val >> 16) & 0xFF, nv5);
        nv5_svga_out(real_address + 3, (val >> 24) & 0xFF, nv5);
        
        return; 
    }

    nv5_mmio_arbitrate_write(addr, val);
}

// PCI stuff
// BAR0         Pointer to MMIO space
// BAR1         Pointer to Linear Framebuffer (NV_USER)

uint8_t nv5_pci_read(int32_t func, int32_t addr, void* priv)
{
    uint8_t ret = 0x00;

    // sanity check
    if (!nv5)
        return ret; 

    // figure out what size this gets read as first
    // seems func does not matter at least here?
    switch (addr) 
    {
        // Get the pci vendor id..

        case NV5_PCI_CFG_VENDOR_ID:
            ret = (PCI_VENDOR_NV & 0xFF);
            break;
        
        case NV5_PCI_CFG_VENDOR_ID + 1: // all access 8bit
            ret = (PCI_VENDOR_NV >> 8);
            break;

        // device id

        case NV5_PCI_CFG_DEVICE_ID:
            ret = (PCI_DEVICE_NV5 & 0xFF);
            break;
        
        case NV5_PCI_CFG_DEVICE_ID+1:
            ret = (PCI_DEVICE_NV5 >> 8);
            break;

        case PCI_REG_COMMAND_L:
            ret = nv5->pci_config.pci_regs[PCI_REG_COMMAND_L] & NV5_PCI_COMMAND_L_IO & NV5_PCI_COMMAND_L_MEMORY & NV5_PCI_COMMAND_MASTER;
            break;
        
        case PCI_REG_COMMAND_H:
            ret = nv5->pci_config.pci_regs[PCI_REG_COMMAND_H];
            break;

        // pci status register
        case PCI_REG_STATUS_L:
            if (nv5->pextdev.straps 
            & NV5_PSTRAPS_BUS_SPEED_66MHZ)
                ret = (nv5->pci_config.pci_regs[PCI_REG_STATUS_L] | NV5_PCI_STATUS_L_CAP_LIST | NV5_PCI_STATUS_L_66MHZ_CAPABLE | NV5_PCI_STATUS_L_FAST_BACK);
            else
                ret = nv5->pci_config.pci_regs[PCI_REG_STATUS_L] & NV5_PCI_STATUS_L_CAP_LIST & NV5_PCI_STATUS_L_FAST_BACK;

            break;

        case PCI_REG_STATUS_H:
            ret = (nv5->pci_config.pci_regs[PCI_REG_STATUS_H]) & (NV5_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING << NV5_PCI_STATUS_H_DEVSEL_TIMING);
            break;
        
        case NV5_PCI_CFG_REVISION:
            ret = nv5->nvbase.gpu_revision;
            break;
       
        case PCI_REG_PROG_IF:
            ret = 0x00;
            break;
            
        case NV5_PCI_CFG_SUBCLASS_CODE:
            ret = 0x00; // nothing
            break;
        
        case NV5_PCI_CFG_CLASS_CODE:
            ret = NV5_PCI_CFG_CLASS_CODE_VGA; // CLASS_CODE_VGA 
            break;
        
        case NV5_PCI_CFG_CACHE_LINE_SIZE:
            ret = NV5_PCI_CFG_CACHE_LINE_SIZE_DEFAULT_FROM_VBIOS;
            break;
        
        case NV5_PCI_CFG_LATENCY_TIMER:
        case NV5_PCI_CFG_HEADER_TYPE:
        case NV5_PCI_CFG_BIST:
            ret = 0x00;
            break;

        // BARs are marked as prefetchable per the datasheet
        case NV5_PCI_CFG_BAR0_L:
        case NV5_PCI_CFG_BAR1_L:
            // only bit that matters is bit 3 (prefetch bit)
            ret =(NV5_PCI_CFG_BAR_PREFETCHABLE_ENABLED << NV5_PCI_CFG_BAR_PREFETCHABLE);
            break;

        // These registers are hardwired to zero per the datasheet
        // Writes have no effect, we can just handle it here though
        case NV5_PCI_CFG_BAR0_BYTE1 ... NV5_PCI_CFG_BAR0_BYTE2:
        case NV5_PCI_CFG_BAR1_BYTE1 ... NV5_PCI_CFG_BAR1_BYTE2:
            ret = 0x00;
            break;

        // MMIO base address
        case NV5_PCI_CFG_BAR0_BASE_ADDRESS:
            ret = nv5->nvbase.bar0_mmio_base >> 24;//8bit value
            break; 

        case NV5_PCI_CFG_BAR1_BASE_ADDRESS:
            ret = nv5->nvbase.bar1_lfb_base >> 24; //8bit value
            break;

        case NV5_PCI_CFG_ENABLE_VBIOS:
            ret = nv5->pci_config.vbios_enabled;
            break;
        
        case NV5_PCI_CFG_INT_LINE:
            ret = nv5->pci_config.int_line;
            break;
        
        case NV5_PCI_CFG_INT_PIN:
            ret = PCI_INTA;
            break;

        case NV5_PCI_CFG_MIN_GRANT:
            ret = NV5_PCI_CFG_MIN_GRANT_DEFAULT;
            break;

        case NV5_PCI_CFG_MAX_LATENCY:
            ret = NV5_PCI_CFG_MAX_LATENCY_DEFAULT;
            break;

        //bar2-5 are not used and hardwired to 0
        case NV5_PCI_CFG_BAR_INVALID_START ... NV5_PCI_CFG_BAR_INVALID_END:
            ret = 0x00;
            break;
            
        case NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_START:
        case NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_END:
            ret = nv5->pci_config.pci_regs[NV5_PCI_CFG_SUBSYSTEM_ID + (addr & 0x03)];
            break;

        default: // by default just return pci_config.pci_regs
            ret = nv5->pci_config.pci_regs[addr];
            break;
        
    }

    nv_log("NV5: nv5_pci_read func=0x%04x addr=0x%04x ret=0x%04x\n", func, addr, ret);
    return ret; 
}

void nv5_pci_write(int32_t func, int32_t addr, uint8_t val, void* priv)
{

    // sanity check
    if (!nv5)
        return; 

    // some addresses are not writable so can't have any effect and can't be allowed to be modified using this code
    // as an example, only the most significant byte of the PCI BARs can be modified
    if (addr >= NV5_PCI_CFG_BAR0_L && addr <= NV5_PCI_CFG_BAR0_BYTE2
    && addr >= NV5_PCI_CFG_BAR1_L && addr <= NV5_PCI_CFG_BAR1_BYTE2)
        return;

    nv_log("NV5: nv5_pci_write func=0x%04x addr=0x%04x val=0x%04x\n", func, addr, val);

    nv5->pci_config.pci_regs[addr] = val;

    switch (addr)
    {
        // standard pci command stuff
        case PCI_REG_COMMAND_L:
            nv5->pci_config.pci_regs[PCI_REG_COMMAND_L] = val;
            // actually update the mappings
            nv5_update_mappings();
            break;
        // pci status register
        case PCI_REG_STATUS_L:
            nv5->pci_config.pci_regs[PCI_REG_STATUS_L] = val | (NV5_PCI_STATUS_L_CAP_LIST | NV5_PCI_STATUS_L_66MHZ_CAPABLE | NV5_PCI_STATUS_L_FAST_BACK);
            break;
        case PCI_REG_STATUS_H:
            nv5->pci_config.pci_regs[PCI_REG_STATUS_H] = val | (NV5_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING << NV5_PCI_STATUS_H_DEVSEL_TIMING);
            break;
        //TODO: ACTUALLY REMAP THE MMIO AND NV_USER
        case NV5_PCI_CFG_BAR0_BASE_ADDRESS:
            nv5->nvbase.bar0_mmio_base = val << 24;
            nv5_update_mappings();
            break; 
        case NV5_PCI_CFG_BAR1_BASE_ADDRESS:
            nv5->nvbase.bar1_lfb_base = val << 24;
            nv5_update_mappings();
            break;
        case NV5_PCI_CFG_ENABLE_VBIOS:
        case NV5_PCI_CFG_VBIOS_BASE:
            
            // make sure we are actually toggling the vbios, not the rom base
            if (addr == NV5_PCI_CFG_ENABLE_VBIOS)
                nv5->pci_config.vbios_enabled = (val & 0x01);

            if (nv5->pci_config.vbios_enabled)
            {
                // First see if we simply wanted to change the VBIOS location

                // Enable it in case it was disabled before
                mem_mapping_enable(&nv5->nvbase.vbios.mapping);

                if (addr != NV5_PCI_CFG_ENABLE_VBIOS)
                {
                    uint32_t old_addr = nv5->nvbase.vbios.mapping.base;
                    // 9bit register
                    uint32_t new_addr = nv5->pci_config.pci_regs[NV5_PCI_CFG_VBIOS_BASE_H] << 24 |
                    nv5->pci_config.pci_regs[NV5_PCI_CFG_VBIOS_BASE_L] << 16;

                    // move it
                    mem_mapping_set_addr(&nv5->nvbase.vbios.mapping, new_addr, 0x10000);

                    nv_log("...i like to move it move it (VBIOS Relocation) 0x%04x -> 0x%04x\n", old_addr, new_addr);

                }
                else
                {
                    nv_log("...VBIOS Enable\n");
                }
            }
            else
            {
                nv_log("...VBIOS Disable\n");
                mem_mapping_disable(&nv5->nvbase.vbios.mapping);

            }
            break;
        case NV5_PCI_CFG_INT_LINE:
            nv5->pci_config.int_line = val;
            break;
        //bar2-5 are not used and can't be written to
        case NV5_PCI_CFG_BAR_INVALID_START ... NV5_PCI_CFG_BAR_INVALID_END:
            break;

        // these are mirrored to the subsystem id and also stored in the ROMBIOS
        case NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_START:
        case NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_END:
            nv5->pci_config.pci_regs[NV5_PCI_CFG_SUBSYSTEM_ID + (addr & 0x03)] = val;
            break;

        default:

    }
}


//
// SVGA functions
//
void nv5_recalc_timings(svga_t* svga)
{    
    // sanity check
    if (!nv5)
        return; 

    nv5_t* nv5 = (nv5_t*)svga->priv;

    svga->ma_latch += (svga->crtc[NV5_CRTC_REGISTER_RPC0] & 0x1F) << 16;
    svga->rowoffset += (svga->crtc[NV5_CRTC_REGISTER_RPC0] & 0xE0) << 3;

    // should these actually use separate values?
    // i don't we should force the top 2 bits to 1...

    // required for VESA resolutions, force parameters higher
    if (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 1 << (NV5_CRTC_REGISTER_FORMAT_VDT10)) svga->vtotal += 0x400;
    if (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 1 << (NV5_CRTC_REGISTER_FORMAT_VDE10)) svga->dispend += 0x400;
    if (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 1 << (NV5_CRTC_REGISTER_FORMAT_VRS10)) svga->vblankstart += 0x400;
    if (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 1 << (NV5_CRTC_REGISTER_FORMAT_VBS10)) svga->vsyncstart += 0x400;
    if (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 1 << (NV5_CRTC_REGISTER_FORMAT_HBE6)) svga->hdisp += 0x400;  

    if (svga->crtc[NV5_CRTC_REGISTER_HEB] & 0x01)
        svga->hdisp += 0x100; // large screen bit

    // Set the pixel mode
    switch (svga->crtc[NV5_CRTC_REGISTER_PIXELMODE] & 0x03)
    {
        ///0x0 is VGA textmode
        case NV5_CRTC_REGISTER_PIXELMODE_8BPP:
            svga->bpp = 8;
            svga->lowres = 0;
            svga->render = svga_render_8bpp_highres;
            break;
        case NV5_CRTC_REGISTER_PIXELMODE_16BPP:
            svga->bpp = 16;
            svga->lowres = 0;
            svga->render = svga_render_16bpp_highres;
            break;
        case NV5_CRTC_REGISTER_PIXELMODE_32BPP:
            svga->bpp = 32;
            svga->lowres = 0;
            svga->render = svga_render_32bpp_highres;
            break;
    }

    // from nv_riva128
    if (((svga->miscout >> 2) & 2) == 2)
    {
        // set clocks
        nv5_pramdac_set_pixel_clock();
        nv5_pramdac_set_vram_clock();
    }
}

void nv5_speed_changed(void* priv)
{
    // sanity check
    if (!nv5)
        return; 
        
    nv5_recalc_timings(&nv5->nvbase.svga);
}

// Force Redraw
// Reset etc.
void nv5_force_redraw(void* priv)
{
    // sanity check
    if (!nv5)
        return; 

    nv5->nvbase.svga.fullchange = changeframecount; 
}

// Read from SVGA core memory
uint8_t nv5_svga_in(uint16_t addr, void* priv)
{

    nv5_t* nv5 = (nv5_t*)priv;

    uint8_t ret = 0x00;

    // sanity check
    if (!nv5)
        return ret; 

    // If we need to RMA from GPU MMIO, go do that
    if (addr >= NV5_RMA_REGISTER_START
    && addr <= NV5_RMA_REGISTER_END)
    {
        if (!(nv5->pbus.rma.mode & 0x01))
            return ret;

        // must be dword aligned
        uint32_t real_rma_read_addr = ((nv5->pbus.rma.mode & NV5_CRTC_REGISTER_RMA_MODE_MAX - 1) << 1) + (addr & 0x03); 
        ret = nv5_pbus_rma_read(real_rma_read_addr);
        return ret;
    }

    // mask off b0/d0 registers 
    if ((((addr & 0xFFF0) == 0x3D0 
    || (addr & 0xFFF0) == 0x3B0) && addr < 0x3de) 
    && !(nv5->nvbase.svga.miscout & 1))
        addr ^= 0x60;

    switch (addr)
    {
        // Alias for "get current SVGA CRTC register ID"
        case 0x3D4:
            ret = nv5->nvbase.svga.crtcreg;
            break;
        case 0x3D5:
            // Support the extended NVIDIA CRTC register range
            switch (nv5->nvbase.svga.crtcreg)
            {
                case NV5_CRTC_REGISTER_RL0:
                    ret = nv5->nvbase.svga.displine & 0xFF; 
                    break;
                    /* Is rl1?*/
                case NV5_CRTC_REGISTER_RL1:
                    ret = (nv5->nvbase.svga.displine >> 8) & 7;
                    break;
                case NV5_CRTC_REGISTER_I2C:
                    ret = i2c_gpio_get_sda(nv5->nvbase.i2c) << 3
                    | i2c_gpio_get_scl(nv5->nvbase.i2c) << 2;

                    break;
                default:
                    ret = nv5->nvbase.svga.crtc[nv5->nvbase.svga.crtcreg];
            }
            break;
        default:
            ret = svga_in(addr, &nv5->nvbase.svga);
            break;
    }

    return ret; //TEMP
}

// Write to SVGA core memory
void nv5_svga_out(uint16_t addr, uint8_t val, void* priv)
{
    // sanity check
    if (!nv5)
        return; 

    // If we need to RMA to GPU MMIO, go do that
    if (addr >= NV5_RMA_REGISTER_START
    && addr <= NV5_RMA_REGISTER_END)
    {
        // we don't need to store these registers...
        nv5->pbus.rma.rma_regs[addr & 3] = val;

        if (!(nv5->pbus.rma.mode & 0x01)) // we are halfway through sending something
            return;

        uint32_t real_rma_write_addr = ((nv5->pbus.rma.mode & (NV5_CRTC_REGISTER_RMA_MODE_MAX - 1)) << 1) + (addr & 0x03); 

        nv5_pbus_rma_write(real_rma_write_addr, nv5->pbus.rma.rma_regs[addr & 3]);
        return;
    }

    // mask off b0/d0 registers 
    if ((((addr & 0xFFF0) == 0x3D0 || (addr & 0xFFF0) == 0x3B0) 
    && addr < 0x3de) 
    && !(nv5->nvbase.svga.miscout & 1))//miscout bit 7 controls mappping
        addr ^= 0x60;

    uint8_t crtcreg = nv5->nvbase.svga.crtcreg;
    uint8_t old_value;

    // todo:
    // Pixel formats (8bit vs 555 vs 565)
    // VBE 3.0?
    
    switch (addr)
    {
        case 0x3D4:
            // real mode access to GPU MMIO space...
            nv5->nvbase.svga.crtcreg = val;
            break;
        // support the extended crtc regs and debug this out
        case 0x3D5:

            // Implements the VGA Protect register
            if ((nv5->nvbase.svga.crtcreg < NV5_CRTC_REGISTER_OVERFLOW) && (nv5->nvbase.svga.crtc[0x11] & 0x80))
                return;

            // Ignore certain bits when VGA Protect register set and we are writing to CRTC register=07h
            if ((nv5->nvbase.svga.crtcreg == NV5_CRTC_REGISTER_OVERFLOW) && (nv5->nvbase.svga.crtc[0x11] & 0x80))
                val = (nv5->nvbase.svga.crtc[NV5_CRTC_REGISTER_OVERFLOW] & ~0x10) | (val & 0x10);

            // set the register value...
            old_value = nv5->nvbase.svga.crtc[crtcreg];

            nv5->nvbase.svga.crtc[crtcreg] = val;
            // ...now act on it

            // Handle nvidia extended Bank0/Bank1 IDs
            switch (crtcreg)
            {
                case NV5_CRTC_REGISTER_READ_BANK:
                        nv5->nvbase.cio_read_bank = val;
                        if (nv5->nvbase.svga.chain4) // chain4 addressing (planar?)
                            nv5->nvbase.svga.read_bank = nv5->nvbase.cio_read_bank << 15;
                        else
                            nv5->nvbase.svga.read_bank = nv5->nvbase.cio_read_bank << 13; // extended bank numbers
                    break;
                case NV5_CRTC_REGISTER_WRITE_BANK:
                    nv5->nvbase.cio_write_bank = val;
                        if (nv5->nvbase.svga.chain4)
                            nv5->nvbase.svga.write_bank = nv5->nvbase.cio_write_bank << 15;
                        else
                            nv5->nvbase.svga.write_bank = nv5->nvbase.cio_write_bank << 13;
                    break;
                case NV5_CRTC_REGISTER_RMA:
                    nv5->pbus.rma.mode = val & NV5_CRTC_REGISTER_RMA_MODE_MAX;
                    break;
                case NV5_CRTC_REGISTER_I2C_GPIO:
                    uint8_t scl = !!(val & 0x20);
                    uint8_t sda = !!(val & 0x10);
                    // Set an I2C GPIO register
                    i2c_gpio_set(nv5->nvbase.i2c, scl, sda);
                    break;
            }

            /* Recalculate the timings if we actually changed them 
            Additionally only do it if the value actually changed*/
            if (old_value != val)
            {
                // Thx to Fuel who basically wrote all the SVGA compatibility code already (although I fixed some issues), because VGA is boring 
                // and in the words of an ex-Rendition/3dfx/NVIDIA engineer, "VGA was basically an undocumented bundle of steaming you-know-what.   
                // And it was essential that any cores the PC 3D startups acquired had to work with all the undocumented modes and timing tweaks (mode X, etc.)"
                if (nv5->nvbase.svga.crtcreg < 0xE
                && nv5->nvbase.svga.crtcreg > 0x10)
                {
                    nv5->nvbase.svga.fullchange = changeframecount;
                    svga_recalctimings(&nv5->nvbase.svga);
                }
            }

            break;
        default:
            svga_out(addr, val, &nv5->nvbase.svga);
            break;
    }

}

void nv5_draw_cursor(svga_t* svga, int32_t drawline)
{
    // sanity check
    if (!nv5)
        return; 
    
    // this is a 2kb bitmap in vram...somewhere...

    nv_log("nv5_draw_cursor drawline=0x%04x", drawline);
}

// Initialise the MMIO mappings
void nv5_init_mappings_mmio()
{
    nv_log("NV5: Initialising 32MB MMIO area\n");

    // 0x0 - 1000000: regs
    // 0x1000000-2000000

    // initialize the mmio mapping
    mem_mapping_add(&nv5->nvbase.mmio_mapping, 0, 0, 
        nv5_mmio_read8,
        nv5_mmio_read16,
        nv5_mmio_read32,
        nv5_mmio_write8,
        nv5_mmio_write16,
        nv5_mmio_write32,
        NULL, MEM_MAPPING_EXTERNAL, nv5);
}

void nv5_init_mappings_svga()
{
    nv_log("NV5: Initialising SVGA core memory mapping\n");

    // setup the svga mappings
    mem_mapping_set(&nv5->nvbase.framebuffer_mapping, 0, 0,
        svga_read_linear,
        svga_readw_linear,
        svga_readl_linear,
        svga_write_linear,
        svga_writew_linear,
        svga_writel_linear,
        NULL, 0, &nv5->nvbase.svga);

    io_sethandler(0x03c0, 0x0020, 
    nv5_svga_in, NULL, NULL, 
    nv5_svga_out, NULL, NULL, 
    nv5);
}

void nv5_init_mappings()
{
    nv5_init_mappings_mmio();
    nv5_init_mappings_svga();
}

// Updates the mappings after initialisation. 
void nv5_update_mappings()
{
    // sanity check
    if (!nv5)
        return; 

    // setting this to 0 doesn't seem to disable it, based on the datasheet

    nv_log("\nMemory Mapping Config Change:\n");

    (nv5->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_IO) ? nv_log("Enable I/O\n") : nv_log("Disable I/O\n");

    io_removehandler(0x03c0, 0x0020, 
        nv5_svga_in, NULL, NULL, 
        nv5_svga_out, NULL, NULL, 
        nv5);

    if (nv5->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_IO)
        io_sethandler(0x03c0, 0x0020, 
        nv5_svga_in, NULL, NULL, 
        nv5_svga_out, NULL, NULL, 
        nv5);   
    
    // turn off bar0 and bar1 by defualt
    mem_mapping_disable(&nv5->nvbase.mmio_mapping);
    mem_mapping_disable(&nv5->nvbase.framebuffer_mapping);
    mem_mapping_disable(&nv5->nvbase.framebuffer_mapping_mirror);

    if (!(nv5->pci_config.pci_regs[PCI_REG_COMMAND]) & PCI_COMMAND_MEM)
    {
        nv_log("NV5: The memory was turned off, not much is going to happen.\n");
        return;
    }

    mem_mapping_enable(&nv5->nvbase.mmio_mapping);
    mem_mapping_enable(&nv5->nvbase.framebuffer_mapping);

    // first map bar0

    nv_log("NV5: BAR0 (MMIO Base) = 0x%08x\n", nv5->nvbase.bar0_mmio_base);

    //mem_mapping_enable(&nv5->nvbase.mmio_mapping); // should have no effect if already enabled

    mem_mapping_set_addr(&nv5->nvbase.mmio_mapping, nv5->nvbase.bar0_mmio_base, NV5_MMIO_SIZE);


    // if this breaks anything, remove it
    // skeptical that 0 is used to disable...
    nv_log("NV5: BAR1 (Linear Framebuffer / NV_USER Base & RAMIN) = 0x%08x\n", nv5->nvbase.bar1_lfb_base);

    mem_mapping_enable(&nv5->nvbase.framebuffer_mapping);

    mem_mapping_set_addr(&nv5->nvbase.framebuffer_mapping, nv5->nvbase.bar1_lfb_base, NV5_LFB_SIZE);

    // Did we change the banked SVGA mode?
    switch (nv5->nvbase.svga.gdcreg[0x06] & 0x0c)
    {
        case NV5_CRTC_BANKED_128K_A0000:
            nv_log("NV5: SVGA Banked Mode = 128K @ A0000h\n");
            mem_mapping_set_addr(&nv5->nvbase.svga.mapping, 0xA0000, 0x20000); // 128kb @ 0xA0000
            nv5->nvbase.svga.banked_mask = 0x1FFFF;
            break;
        case NV5_CRTC_BANKED_64K_A0000:
            nv_log("NV5: SVGA Banked Mode = 64K @ A0000h\n");
            mem_mapping_set_addr(&nv5->nvbase.svga.mapping, 0xA0000, 0x10000); // 64kb @ 0xA0000
            nv5->nvbase.svga.banked_mask = 0xFFFF;
            break;
        case NV5_CRTC_BANKED_32K_B0000:
            nv_log("NV5: SVGA Banked Mode = 32K @ B0000h\n");
            mem_mapping_set_addr(&nv5->nvbase.svga.mapping, 0xB0000, 0x8000); // 32kb @ 0xB0000
            nv5->nvbase.svga.banked_mask = 0x7FFF;
            break;
        case NV5_CRTC_BANKED_32K_B8000:
            nv_log("NV5: SVGA Banked Mode = 32K @ B8000h\n");
            mem_mapping_set_addr(&nv5->nvbase.svga.mapping, 0xB8000, 0x8000); // 32kb @ 0xB8000
            nv5->nvbase.svga.banked_mask = 0x7FFF;
            break;
    }
}



// 
// Init code
//
void* nv5_init(const device_t *info)
{
    nv5->nvbase.log = log_open("NV5");

    // Allows nv_log to be used for multiple nvidia devices
    nv_log_set_device(nv5->nvbase.log);    
    nv_log("NV5: initialising core\n");

    // Figure out which vbios the user selected
    const char* vbios_id = device_get_config_bios("VBIOS");
    const char* vbios_file = "";

    // depends on the bus we are using
    if (nv5->nvbase.bus_generation == nv_bus_pci)
        vbios_file = device_get_bios_file(&nv5_device_pci, vbios_id, 0);
    else   
        vbios_file = device_get_bios_file(&nv5_device_agp, vbios_id, 0);

    int32_t err = rom_init(&nv5->nvbase.vbios, vbios_file, 0xC0000, 0x10000, 0xffff, 0, MEM_MAPPING_EXTERNAL);
    
    if (err)
    {
        nv_log("NV5 FATAL: failed to load VBIOS err=%d\n", err);
        fatal("Nvidia NV5 init failed: Somehow selected a nonexistent VBIOS? err=%d\n", err);
        return NULL;
    }
    else    
        nv_log("NV5: Successfully loaded VBIOS %s located at %s\n", vbios_id, vbios_file);

    // set the vram amount and gpu revision
    uint32_t vram_amount = device_get_config_int("VRAM");
    nv5->nvbase.gpu_revision = device_get_config_int("Chip Revision");
    
    // set up the bus and start setting up SVGA core
    if (nv5->nvbase.bus_generation == nv_bus_pci)
    {
        nv_log("NV5: using PCI bus\n");

        pci_add_card(PCI_ADD_NORMAL, nv5_pci_read, nv5_pci_write, NULL, &nv5->nvbase.pci_slot);

        svga_init(&nv5_device_pci, &nv5->nvbase.svga, nv5, vram_amount, 
        nv5_recalc_timings, nv5_svga_in, nv5_svga_out, nv5_draw_cursor, NULL);
    }
    else if (nv5->nvbase.bus_generation == nv_bus_agp_4x)
    {
        nv_log("NV5: using AGP 4X bus\n");

        pci_add_card(PCI_ADD_AGP, nv5_pci_read, nv5_pci_write, NULL, &nv5->nvbase.pci_slot);

        svga_init(&nv5_device_agp, &nv5->nvbase.svga, nv5, vram_amount, 
        nv5_recalc_timings, nv5_svga_in, nv5_svga_out, nv5_draw_cursor, NULL);
    }

    // set vram
    nv_log("NV5: VRAM=%d bytes\n", nv5->nvbase.svga.vram_max);

    // init memory mappings
    nv5_init_mappings();

    // make us actually exist
    nv5->pci_config.int_line = 0xFF;
    nv5->pci_config.pci_regs[PCI_REG_COMMAND] = PCI_COMMAND_IO | PCI_COMMAND_MEM;

    // svga is done, so now initialise the real gpu
    nv_log("NV5: Initialising GPU core...\n");

    nv5_pextdev_init();             // Initialise Straps
    nv5_pmc_init();                 // Initialise Master Control
    nv5_pbus_init();                // Initialise Bus (the 128 part of riva)
    nv5_pfb_init();                 // Initialise Framebuffer Interface
    nv5_pramdac_init();             // Initialise RAMDAC (CLUT, final pixel presentation etc)
    nv5_pfifo_init();               // Initialise FIFO for graphics object submission
    nv5_pgraph_init();              // Initialise accelerated graphics engine
    nv5_ptimer_init();              // Initialise programmable interval timer
    nv5_pvideo_init();              // Initialise video overlay engine

    nv_log("NV5: Initialising I2C...");
    nv5->nvbase.i2c = i2c_gpio_init("nv5_i2c");
    nv5->nvbase.ddc = ddc_init(i2c_gpio_get_bus(nv5->nvbase.i2c));

    return nv5;
}

// This function simply allocates ram and sets the bus to pci before initialising.
void* nv5_init_pci(const device_t* info)
{
    nv5 = (nv5_t*)calloc(1, sizeof(nv5_t));
    nv5->nvbase.bus_generation = nv_bus_pci;
    nv5_init(info);
}

// This function simply allocates ram and sets the bus to agp before initialising.
void* nv5_init_agp(const device_t* info)
{
    nv5 = (nv5_t*)calloc(1, sizeof(nv5_t));
    nv5->nvbase.bus_generation = nv_bus_agp_4x;
    nv5_init(info);
}

void nv5_close(void* priv)
{
    // Shut down logging
    log_close(nv5->nvbase.log);
    nv_log_set_device(NULL);

    // Shut down I2C and the DDC
    ddc_close(nv5->nvbase.ddc);
    i2c_gpio_close(nv5->nvbase.i2c);

    // Destroy the Rivatimers. (It doesn't matter if they are running.)
    rivatimer_destroy(nv5->nvbase.pixel_clock_timer);
    rivatimer_destroy(nv5->nvbase.memory_clock_timer);
    
    // Shut down SVGA
    svga_close(&nv5->nvbase.svga);
    free(nv5);
    nv5 = NULL;
}

// See if the bios rom is available.
int32_t nv5_available()
{
    return rom_present(NV5_VBIOS_UNKNOWN)
    || rom_present(NV5_VBIOS_ASUS_AGP_V3800M)
    || rom_present(NV5_VBIOS_POWERCOLOR_CM64A)
    || rom_present(NV5_VBIOS_CREATIVE_CT6984)
    || rom_present(NV5_VBIOS_INNOVISION_TNT2_M64)
    || rom_present(NV5_VBIOS_LEADTEK_16MB)
    || rom_present(NV5_VBIOS_MANLI_RIVA_TNT2_M64)
    || rom_present(NV5_VBIOS_MSI_MS_8808)
    || rom_present(NV5_VBIOS_PINE_PV_502A_BR)
    || rom_present(NV5_VBIOS_SPARKLE)
    || rom_present(NV5_VBIOS_LEADTEK_WINFAST_3D_325);
}

// NV5 (RIVA TNT2 M64)
// PCI
// 16MB or 32MB VRAM
const device_t nv5_device_pci = 
{
    .name = "NVidia RIVA TNT2 Model 64 (NV5) PCI",
    .internal_name = "nv5",
    .flags = DEVICE_PCI,
    .local = 0,
    .init = nv5_init_pci,
    .close = nv5_close,
    .speed_changed = nv5_speed_changed,
    .force_redraw = nv5_force_redraw,
    .available = nv5_available,
    .config = nv5_config,
};

// NV5 (RIVA TNT2 M64)
// AGP
// 16MB or 32MB VRAM
const device_t nv5_device_agp = 
{
    .name = "NVidia RIVA TNT2 Model 64 (NV5) AGP",
    .internal_name = "nv5_agp",
    .flags = DEVICE_AGP,
    .local = 0,
    .init = nv5_init_agp,
    .close = nv5_close,
    .speed_changed = nv5_speed_changed,
    .force_redraw = nv5_force_redraw,
    .available = nv5_available,
    .config = nv5_config,
};