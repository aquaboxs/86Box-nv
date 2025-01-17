/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          The insane NV5 MMIO arbiter.
 *          Writes to ALL sections of the GPU based on the write position
 *          All writes are internally considered to be 32-bit! Be careful...
 * 
 *          Also handles interrupt dispatch
 *
 *          
 *
 * Authors: aquaboxs, <aquaboxstudios87@gmail.com>
 *
 *          Copyright 2025 aquaboxs
 */

// STANDARD NV5 includes
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

// Gets a register...
// move this somewhere else when we have more models
nv_register_t* nv_get_register(uint32_t address, nv_register_t* register_list, uint32_t num_regs)
{
    for (int32_t reg_num = 0; reg_num < num_regs; reg_num++)
    {
        if (register_list[reg_num].address == NV_REG_LIST_END)
            break; //unimplemented

        if (register_list[reg_num].address == address)
            return &register_list[reg_num];
    }

    return NULL;
}

// Arbitrates an MMIO read
uint32_t nv5_mmio_arbitrate_read(uint32_t address)
{
    // sanity check
    if (!nv3)
        return 0x00; 

    uint32_t ret = 0x00;

    // note: some registers are byte aligned not dword aligned
    // only very few are though, so they can be handled specially, using the register list most likely
    address &= 0xFFFFFC;

    // gigantic set of if statements to send the write to the right subsystem
    if (address >= NV5_PMC_START && address <= NV5_PMC_END)
        ret = nv5_pmc_read(address);
    else if (address >= NV5_CIO_START && address <= NV5_CIO_END)
        ret = nv5_cio_read(address);
    else if (address >= NV5_PBUS_PCI_START && address <= NV5_PBUS_PCI_END)
        ret = nv5_pci_read(0x00, address & 0xFF, NULL);
    else if (address >= NV5_PBUS_START && address <= NV5_PBUS_END)
        ret = nv5_pbus_read(address);
    else if (address >= NV5_PFIFO_START && address <= NV5_PFIFO_END)
        ret = nv5_pfifo_read(address);
    else if (address >= NV5_PFB_START && address <= NV5_PFB_END)
        ret = nv5_pfb_read(address);
    else if (address >= NV5_PRM_START && address <= NV5_PRM_END)
        ret = nv5_prm_read(address);
    else if (address >= NV5_PRMIO_START && address <= NV5_PRMIO_END)
        ret = nv5_prmio_read(address);
    else if (address >= NV5_PTIMER_START && address <= NV5_PTIMER_END)
        ret = nv5_ptimer_read(address);
    else if (address >= NV5_PFB_START && address <= NV5_PFB_END)
        ret = nv5_pfb_read(address);
    else if (address >= NV5_PEXTDEV_START && address <= NV5_PEXTDEV_END)
        ret = nv5_pextdev_read(address);
    else if (address >= NV5_PROM_START && address <= NV5_PROM_END)
        ret = nv5_prom_read(address);
    else if (address >= NV5_PALT_START && address <= NV5_PALT_END)
        ret = nv5_palt_read(address);
    else if (address >= NV5_PME_START && address <= NV5_PME_END)
        ret = nv5_pme_read(address);
    else if (address >= NV5_PGRAPH_START && address <= NV5_PGRAPH_REAL_END) // what we're actually doing here determined by nv5_pgraph_* func
        ret = nv5_pgraph_read(address);
    else if (address >= NV5_PRMCIO_START && address <= NV5_PRMCIO_END)
        ret = nv5_prmcio_read(address);    
    else if (address >= NV5_PVIDEO_START && address <= NV5_PVIDEO_END)
        ret = nv5_pvideo_read(address);
    else if (address >= NV5_PRAMDAC_START && address <= NV5_PRAMDAC_END)
        ret = nv5_pramdac_read(address);
    else if (address >= NV5_VRAM_START && address <= NV5_VRAM_END)
        ret = nv5_vram_read(address);
    else if (address >= NV5_USER_START && address <= NV5_USER_END)
        ret = nv5_user_read(address);
    // RAMIN is handled by a separate memory mapping in PCI BAR1
    //else if (address >= NV5_PRAMIN_START && address <= NV5_PRAMIN_END)
        //ret = nv5_pramin_arbitrate_read(address); // RAMHT, RAMFC, RAMRO etc dettermined by nv5_ramin_* function
    else 
    {
        nv_log("NV3: MMIO read arbitration failed, INVALID address NOT mapped to any GPU subsystem 0x%08x [returning 0x00]\n", address);
        return 0x00;
    }

    return ret;
}

void nv5_mmio_arbitrate_write(uint32_t address, uint32_t value)
{
    // sanity check
    if (!nv3)
        return; 

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    address &= 0xFFFFFF;

    // note: some registers are byte aligned not dword aligned
    // only very few are though, so they can be handled specially, using the register list most likely
    address &= 0xFFFFFC;

    // gigantic set of if statements to send the write to the right subsystem
    if (address >= NV5_PMC_START && address <= NV5_PMC_END)
        nv5_pmc_write(address, value);
    else if (address >= NV5_CIO_START && address <= NV5_CIO_END)
        nv5_cio_write(address, value);
    else if (address >= NV5_PBUS_PCI_START && address <= NV5_PBUS_PCI_END)              // PCI mirrored at 0x1800 in MMIO
        nv5_pci_write(0x00, address & 0xFF, value, NULL); // priv does not matter
    else if (address >= NV5_PBUS_START && address <= NV5_PBUS_END)
        nv5_pbus_write(address, value);
    else if (address >= NV5_PFIFO_START && address <= NV5_PFIFO_END)
        nv5_pfifo_write(address, value);
    else if (address >= NV5_PRM_START && address <= NV5_PRM_END)
        nv5_prm_write(address, value);
    else if (address >= NV5_PRMIO_START && address <= NV5_PRMIO_END)
        nv5_prmio_write(address, value);
    else if (address >= NV5_PTIMER_START && address <= NV5_PTIMER_END)
        nv5_ptimer_write(address, value);
    else if (address >= NV5_PFB_START && address <= NV5_PFB_END)
        nv5_pfb_write(address, value);
    else if (address >= NV5_PEXTDEV_START && address <= NV5_PEXTDEV_END)
        nv5_pextdev_write(address, value);
    else if (address >= NV5_PROM_START && address <= NV5_PROM_END)
        nv5_prom_write(address, value);
    else if (address >= NV5_PALT_START && address <= NV5_PALT_END)
        nv5_palt_write(address, value);
    else if (address >= NV5_PME_START && address <= NV5_PME_END)
        nv5_pme_write(address, value);
    else if (address >= NV5_PGRAPH_START && address <= NV5_PGRAPH_REAL_END) // what we're actually doing here is determined by the nv5_pgraph_* functions
        nv5_pgraph_write(address, value);
    else if (address >= NV5_PRMCIO_START && address <= NV5_PRMCIO_END)
        nv5_prmcio_write(address, value);
    else if (address >= NV5_PVIDEO_START && address <= NV5_PVIDEO_END)
        nv5_pvideo_write(address, value);
    else if (address >= NV5_PRAMDAC_START && address <= NV5_PRAMDAC_END)
        nv5_pramdac_write(address, value);
    else if (address >= NV5_VRAM_START && address <= NV5_VRAM_END)
        nv5_vram_write(address, value);
    else if (address >= NV5_USER_START && address <= NV5_USER_END)
        nv5_user_write(address, value);
    else if (address >= NV5_PRAMIN_START && address <= NV5_PRAMIN_END)
        nv5_pramin_arbitrate_write(address, value); // RAMHT, RAMFC, RAMRO etc is determined by the nv5_ramin_* functions
    else 
    {
        nv_log("NV3: MMIO write arbitration failed, INVALID address NOT mapped to any GPU subsystem 0x%08x\n", address);
        return;
    }
}


//                                                              //
// ******* DUMMY FUNCTIONS FOR UNIMPLEMENTED SUBSYSTEMS ******* //
//                                                              //

// Read and Write functions for GPU subsystems
// Remove the ones that aren't used here eventually, have all of htem for now
uint32_t    nv5_cio_read(uint32_t address) { return 0; };
void        nv5_cio_write(uint32_t address, uint32_t value) {};
uint32_t    nv5_prm_read(uint32_t address) { return 0; };
void        nv5_prm_write(uint32_t address, uint32_t value) {};
uint32_t    nv5_prmio_read(uint32_t address) { return 0; };
void        nv5_prmio_write(uint32_t address, uint32_t value) {};
uint32_t    nv5_prom_read(uint32_t address) { return 0; };
void        nv5_prom_write(uint32_t address, uint32_t value) {};
uint32_t    nv5_palt_read(uint32_t address) { return 0; };
void        nv5_palt_write(uint32_t address, uint32_t value) {};

// TODO: PGRAPH class registers
uint32_t    nv5_prmcio_read(uint32_t address) { return 0; };
void        nv5_prmcio_write(uint32_t address, uint32_t value) {};

uint32_t    nv5_vram_read(uint32_t address) { return 0; };
void        nv5_vram_write(uint32_t address, uint32_t value) {};

uint32_t    nv5_user_read(uint32_t address) { return 0; };
void        nv5_user_write(uint32_t address, uint32_t value) {};