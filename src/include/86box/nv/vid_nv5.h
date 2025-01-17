/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 * 
 *          Welcome to what happens when a single person demands that their overly complicated "vision" of a design be implemented 
 *          with absolutely no compromise. This is true lunacy.
 * 
 *          Explanation of what is being done here, and how this all works, hopefully posted on the 86Box blog.
 *          Notes specific to a subsystem in the header or c file for that subsystem
 *          Also check the doc folder for some more notres
 * 
 *          vid_nv5.h:      NV5 Architecture Hardware Reference (open-source)
 *          Last updated:   17 January 2025
 *
 * Authors: aquaboxs <aquaboxstudios87@gmail.com>
 *
 *          Copyright 2025 aquaboxs
 */

#pragma once
#include <86Box/nv/classes/vid_nv5_classes.h>

// The GPU base structure
extern const device_config_t nv5_config[];

#define NV5_MMIO_SIZE                                   0x1000000       // Max MMIO size
#define NV5_LFB_SIZE                                    0x2000000       // Size of LFB

#define NV5_LFB_RAMIN_MIRROR_START                      0x400000        // Mirror of ramin (VERIFY ON HARDWARE)
#define NV5_LFB_2NDHALF_START                           0x800000        // The second half of LFB(?)
#define NV5_LFB_RAMIN_START                             0xC00000        // RAMIN mapping start
#define NV5_LFB_MAPPING_SIZE                            0x400000        // Size of RAMIN

// DMA channels are basically the number of contexts that the gpu can deal with at once.
// Channel 0 is always taken up by NV drivers.

// Subchannels deal with specific parts of the GPU and are manipulated by the driver to manipulate the gpu.
#define NV5_DMA_CHANNELS                                8
#define NV5_DMA_SUBCHANNELS_PER_CHANNEL                 8

#define NV5_86BOX_TIMER_SYSTEM_FIX_QUOTIENT             10              // The amount by which we have to ration out the memory clock because it's not fast enough...
                                                                        // Multiply by this value to get the real clock speed.
// Default value for the boot information register.
// Depends on the chip
#define NV5_BOOT_REG_REV_A00                            0x20154000
#define NV5_BOOT_REG_REV_B00                            0x20154010
#define NV5_BOOT_REG_REV_C00                            0x20154020

// various vbioses for testing

#define NV5_VBIOS_UNKNOWN                         "roms/video/nvidia/nv5/62090211.rom"
#define NV5_VBIOS_ASUS_AGP_V3800M                 "roms/video/nvidia/nv5/asus_125-143mhz32mb.rom"
#define NV5_VBIOS_POWERCOLOR_CM64A                "roms/video/nvidia/nv5/cm64a_120-125mhz.rom"
#define NV5_VBIOS_CREATIVE_CT6984                 "roms/video/nvidia/nv5/creativect6980_125-125mhz.rom"
#define NV5_VBIOS_INNOVISION_TNT2_M64             "roms/video/nvidia/nv5/inno3dpci_125-125mhz.bin"
#define NV5_VBIOS_LEADTEK_16MB                    "roms/video/nvidia/nv5/leadtek16mb_125-143mhz.rom"
#define NV5_VBIOS_MANLI_RIVA_TNT2_M64             "roms/video/nvidia/nv5/manli_135-135mhz.rom"
#define NV5_VBIOS_MSI_MS_8808                     "roms/video/nvidia/nv5/msi_ms-8808_125-150mhz.rom"
#define NV5_VBIOS_PINE_PV_502A_BR                 "roms/video/nvidia/nv5/pine_pv-t02a-br_125-143mhz.rom"
#define NV5_VBIOS_SPARKLE                         "roms/video/nvidia/nv5/sparkle_sp5300_125-125mhz.rom"
#define NV5_VBIOS_LEADTEK_WINFAST_3D_325          "roms/video/nvidia/nv5/w2137.rom"

// The default VBIOS to use
#define NV5_VBIOS_DEFAULT                               NV5_VBIOS_POWERCOLOR_CM64A

// Temporary, will be loaded from settings
#define VRAM_SIZE_2MB                                   0x200000 // 2MB
#define VRAM_SIZE_4MB                                   0x400000 // 4MB
#define VRAM_SIZE_8MB                                   0x800000 // NV3T only


// PCI config
#define NV5_PCI_CFG_VENDOR_ID                           0x0
#define NV5_PCI_CFG_DEVICE_ID                           0x2
#define NV5_PCI_CFG_CAPABILITIES                        0x4

#define NV5_PCI_COMMAND_L_IO                            1
#define NV5_PCI_COMMAND_L_IO_ENABLED                    0x1
#define NV5_PCI_COMMAND_L_MEMORY                        2
#define NV5_PCI_COMMAND_L_MEMORY_ENABLED                0x1
#define NV5_PCI_COMMAND_MASTER  0x4

#define NV5_PCI_COMMAND_H_FAST_BACK2BACK                0x01

#define NV5_PCI_STATUS_L_66MHZ_CAPABLE                  0x20
#define NV5_PCI_STATUS_L_FAST_BACK                      0x80
#define NV5_PCI_STATUS_L_CAP_LIST                       0x10
#define NV5_PCI_STATUS_H_DEVSEL_TIMING                  5
#define NV5_PCI_STATUS_H_FAST_DEVSEL_TIMING             0x00
#define NV5_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING           0x02

#define NV5_PCI_CFG_REVISION                            0x8

#define NV5_PCI_CFG_REVISION_DEFAULT                    0x15

#define NV5_PCI_CFG_PROGRAMMING_INTERFACE               0x9
#define NV5_PCI_CFG_SUBCLASS_CODE                       0x0A
#define NV5_PCI_CFG_CLASS_CODE                          0x0B
#define NV5_PCI_CFG_CLASS_CODE_VGA                      0x03

#define NV5_PCI_CFG_CACHE_LINE_SIZE                     0x0C
#define NV5_PCI_CFG_CACHE_LINE_SIZE_DEFAULT_FROM_VBIOS  0x40

#define NV5_PCI_CFG_LATENCY_TIMER                       0x0D
#define NV5_PCI_CFG_HEADER_TYPE                         0x0E
#define NV5_PCI_CFG_BIST                                0x0F

// PCI Bars
#define NV5_PCI_CFG_BAR_PREFETCHABLE                    3
#define NV5_PCI_CFG_BAR_PREFETCHABLE_ENABLED            0x1

#define NV5_PCI_CFG_BAR0_L                              0x10
#define NV5_PCI_CFG_BAR0_BYTE1                          0x11
#define NV5_PCI_CFG_BAR0_BYTE2                          0x12
#define NV5_PCI_CFG_BAR0_BASE_ADDRESS                   0x13
#define NV5_PCI_CFG_BAR1_L                              0x14
#define NV5_PCI_CFG_BAR1_BYTE1                          0x15
#define NV5_PCI_CFG_BAR1_BYTE2                          0x16
#define NV5_PCI_CFG_BAR1_BASE_ADDRESS                   0x17
#define NV5_PCI_CFG_BAR_INVALID_START                   0x18
#define NV5_PCI_CFG_BAR_INVALID_END                     0x27
#define NV5_PCI_CFG_SUBSYSTEM_ID                        0x2C

#define NV5_PCI_CFG_ENABLE_VBIOS                        0x30
#define NV5_PCI_CFG_VBIOS_BASE                          0x32 ... 0x33
#define NV5_PCI_CFG_VBIOS_BASE_L                        0x32
#define NV5_PCI_CFG_VBIOS_BASE_H                        0x33

#define NV5_PCI_CFG_INT_LINE                            0x3C
#define NV5_PCI_CFG_INT_PIN                             0x3D

#define NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_START           0x40
#define NV5_PCI_CFG_SUBSYSTEM_ID_MIRROR_END             0x43

#define NV5_PCI_CFG_MIN_GRANT                           0x3E
#define NV5_PCI_CFG_MIN_GRANT_DEFAULT                   0x03
#define NV5_PCI_CFG_MAX_LATENCY                         0x3F
#define NV5_PCI_CFG_MAX_LATENCY_DEFAULT                 0x01

// GPU Subsystems
// These most likely correspond to functional blocks in the original design

#define NV5_PMC_START                                   0x0         // Chip Master Control Subsystem

#define NV5_PMC_BOOT                                    0x0         // Boot Configuration

#define NV5_PMC_INTERRUPT_STATUS                        0x100       // Interrupt Control
#define NV5_PMC_INTERRUPT_PAUDIO                        0           // Unused, NV3A only
#define NV5_PMC_INTERRUPT_PAUDIO_PENDING                0x1         // Unused, NV3A only
#define NV5_PMC_INTERRUPT_PMEDIA                        4
#define NV5_PMC_INTERRUPT_PMEDIA_PENDING                0x1
#define NV5_PMC_INTERRUPT_PFIFO                         8
#define NV5_PMC_INTERRUPT_PFIFO_PENDING                 0x1
#define NV5_PMC_INTERRUPT_PGRAPH0                       12
#define NV5_PMC_INTERRUPT_PGRAPH0_PENDING               0x1
#define NV5_PMC_INTERRUPT_PGRAPH1                       13
#define NV5_PMC_INTERRUPT_PGRAPH1_PENDING               0x1
#define NV5_PMC_INTERRUPT_PVIDEO                        16
#define NV5_PMC_INTERRUPT_PVIDEO_PENDING                0x1
#define NV5_PMC_INTERRUPT_PTIMER                        20
#define NV5_PMC_INTERRUPT_PTIMER_PENDING                0x1
#define NV5_PMC_INTERRUPT_PFB                           24
#define NV5_PMC_INTERRUPT_PFB_PENDING                   0x1
#define NV5_PMC_INTERRUPT_PBUS                          28  
#define NV5_PMC_INTERRUPT_PBUS_PENDING                  0x1
#define NV5_PMC_INTERRUPT_SOFTWARE                      31
#define NV5_PMC_INTERRUPT_SOFTWARE_PENDING              0x1
#define NV5_PMC_INTERRUPT_ENABLE                        0x140       // Controls global interrupt enable state
#define NV5_PMC_INTERRUPT_ENABLE_HARDWARE               0x1         // Determines if hardware interrupts are enabled
#define NV5_PMC_INTERRUPT_ENABLE_SOFTWARE               0x2         // Determinse if software interrupts were enabled
#define NV5_PMC_ENABLE                                  0x200       // Determines which gpu subsystems were enabled
#define NV5_PMC_ENABLE_PAUDIO                           0           // UNUSED - PAudio removed in NV3 Stepping B0 
#define NV5_PMC_ENABLE_PAUDIO_ENABLED                   0x1         // UNUSED - PAudio removed in NV3 Stepping B0
#define NV5_PMC_ENABLE_PMEDIA                           4
#define NV5_PMC_ENABLE_PMEDIA_ENABLED                   0x1
#define NV5_PMC_ENABLE_PFIFO                            8
#define NV5_PMC_ENABLE_PFIFO_ENABLED                    0x1
#define NV5_PMC_ENABLE_PGRAPH                           12          // Determines if PGRAPH is enabled.
#define NV5_PMC_ENABLE_PGRAPH_ENABLED                   0x1
#define NV5_PMC_ENABLE_PPMI                             16
#define NV5_PMC_ENABLE_PPMI_ENABLED                     0x1
#define NV5_PMC_ENABLE_PFB                              20
#define NV5_PMC_ENABLE_PFB_ENABLED                      0x1
#define NV5_PMC_ENABLE_PCRTC                            24
#define NV5_PMC_ENABLE_PCRTC_ENABLED                    0x1
#define NV5_PMC_ENABLE_PVIDEO                           28
#define NV5_PMC_ENABLE_PVIDEO_ENABLED                   0x1

#define NV5_PMC_END                                     0xfff       // overlaps with CIO
#define NV5_CIO_START                                   0x3b0       // Legacy SVGA Emulation Subsystem
#define NV5_CIO_END                                     0x3df
#define NV5_PBUS_START                                  0x1000      // Bus Control Subsystem
#define NV5_PBUS_INTR                                   0x1100      // Bus Control - Interrupt Status
#define NV5_PBUS_INTR_EN                                0x1140      // Bus Control - Interrupt Enable
#define NV5_PBUS_PCI_START                              0x1800      // PCI mirror start
#define NV5_PBUS_PCI_END                                0x18FF      // PCI mirror end
#define NV5_PBUS_END                                    0x1FFF
#define NV5_PFIFO_START                                 0x2000      // FIFO for DMA Object Submission (uses hashtable to store the objects)

#define NV5_PFIFO_INTR                                  0x2100      // FIFO - Interrupt Status
#define NV5_PFIFO_INTR_EN                               0x2140      // FIFO - Interrupt Enable

#define NV5_PFIFO_CONFIG_0                              0x2200
#define NV5_PFIFO_CONFIG_0_DMA_FETCH                    8

#define NV5_PFIFO_CONFIG_RAMHT                          0x2210      // Hashtable for graphics objects config
#define NV5_PFIFO_CONFIG_RAMHT_BASE_ADDRESS             12
#define NV5_PFIFO_CONFIG_RAMHT_BASE_ADDRESS_DEFAULT     0x0
#define NV5_PFIFO_CONFIG_RAMHT_SIZE                     16
#define NV5_PFIFO_CONFIG_RAMHT_SIZE_4K                  0x0
#define NV5_PFIFO_CONFIG_RAMHT_SIZE_8K                  0x1
#define NV5_PFIFO_CONFIG_RAMHT_SIZE_16K                 0x2
#define NV5_PFIFO_CONFIG_RAMHT_SIZE_32K                 0x3

#define NV5_PFIFO_CONFIG_RAMFC                          0x2214
#define NV5_PFIFO_CONFIG_RAMFC_BASE_ADDRESS             9
#define NV5_PFIFO_CONFIG_RAMFC_BASE_ADDRESS_DEFAULT     0x1C00      // Hardcoded in silicon?

#define NV5_PFIFO_CONFIG_RAMRO                          0x2218
#define NV5_PFIFO_CONFIG_RAMRO_BASE_ADDRESS             9
#define NV5_PFIFO_CONFIG_RAMRO_BASE_ADDRESS_DEFAULT     0x1E00      // Hardcoded in silicon?
#define NV5_PFIFO_CONFIG_RAMRO_SIZE                     16
#define NV5_PFIFO_CONFIG_RAMRO_SIZE_512B                0x0
#define NV5_PFIFO_CONFIG_RAMRO_SIZE_8K                  0x1 

#define NV5_PFIFO_END                                   0x3FFF
#define NV5_PRM_START                                   0x4000      // Real-Mode Device Support Subsystem
#define NV5_PRM_INTR                                    0x4100
#define NV5_PRM_INTR_EN                                 0x4140
#define NV5_PRM_END                                     0x4FFF
#define NV5_PRAM_START                                  0x6000      // Local ram/cache?
#define NV5_PRAM_END                                    0x6FFF
#define NV5_PRMIO_START                                 0x7000      // Real-Mode I/O Subsystem
#define NV5_PRMIO_END                                   0x7FFF
#define NV5_PTIMER_START                                0x9000      // Programmable Interval Timer
#define NV5_PTIMER_INTR                                 0x9100
#define NV5_PTIMER_INTR_ALARM                           0           // Alarm interrupt
#define NV5_PTIMER_INTR_EN                              0x9140
#define NV5_PTIMER_NUMERATOR                            0x9200
#define NV5_PTIMER_DENOMINATOR                          0x9210
#define NV5_PTIMER_TIME_0_NSEC                          0x9400      // nanoseconds [31:5]
#define NV5_PTIMER_TIME_1_NSEC                          0x9410      // nanoseconds [28:0]
#define NV5_PTIMER_ALARM_NSEC                           0x9420      // nanoseconds [31:5]
#define NV5_PTIMER_END                                  0x9FFF
#define NV5_VGA_VRAM_START                              0xA0000     // VGA Emulation VRAM
#define NV5_VGA_VRAM_END                                0xBFFFF
#define NV5_VGA_START                                   0xC0000     // VGA Emulation Registers
#define NV5_VGA_END                                     0xC7FFF
#define NV5_PRMVIO_START                                NV5_VGA_START // VGA stuff written from main GPU
#define NV5_PRMVIO_END                                  0xC0400
#define NV5_PFB_START                                   0x100000    // GPU Interface to VRAM
#define NV5_PFB_BOOT                                    0x100000    // Boot registration 
#define NV5_PFB_BOOT_RAM_AMOUNT                         0           // The amount of ram
#define NV5_PFB_BOOT_RAM_AMOUNT_32MB                    0x0
#define NV5_PFB_BOOT_RAM_AMOUNT_4MB                     0x1
#define NV5_PFB_BOOT_RAM_AMOUNT_8MB                     0x2
#define NV5_PFB_BOOT_RAM_AMOUNT_16MB                    0x3
#define NV5_PFB_BOOT_RAM_WIDTH                          2           // the bus width of the gpu's vram
#define NV5_PFB_BOOT_RAM_WIDTH_64                       0x0         // 64bit
#define NV5_PFB_BOOT_RAM_WIDTH_128                      0x1         // 128bit
#define NV5_PFB_BOOT_RAM_BANKS                          3           // the number of banks
#define NV5_PFB_BOOT_RAM_BANKS_2                        0x0         // 2 banks (seems to be used for 2mb)
#define NV5_PFB_BOOT_RAM_BANKS_4                        0x1         // 4 banks (seems to be used for 4mb)
#define NV5_PFB_BOOT_RAM_DATA_TWIDDLE                   4
#define NV5_PFB_BOOT_RAM_DATA_TWIDDLE_OFF               0x0
#define NV5_PFB_BOOT_RAM_DATA_TWIDDLE_ON                0x1
#define NV5_PFB_BOOT_RAM_EXTENSION                      5
#define NV5_PFB_BOOT_RAM_EXTENSION_NONE                 0x0
#define NV5_PFB_BOOT_RAM_EXTENSION_8MB                  0x1
#define NV5_PFB_CONFIG_0                                0x100200    // Framebuffer interface config register 0
#define NV5_PFB_CONFIG_0_RESOLUTION                     0
// 1=40 horiz. resolution
// i assume it can be divided by some kind of divisor to produce the vertical resolution (e.g. 3/2 or multiply by 2/3) to get the final 
// horiz is 32*value
// theoretically it should support resolutions from 40-2560 horiz

// WHAT ARE THE TIMINGS: ARE THEY IN THE VBIOS?
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_320           0xA
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_400           0xD
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_480           0xF
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_512           0x10
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_640           0x14
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_800           0x19
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_960           0x1E
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_1024          0x20
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_1152          0x24
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_1280          0x28
#define NV5_PFB_CONFIG_0_HORIZ_RESOLUTION_1600          0x32

#define NV5_PFB_CONFIG_0_PIXEL_DEPTH                    8
#define NV5_PFB_CONFIG_0_DEPTH_8BPP                     0x1
#define NV5_PFB_CONFIG_0_DEPTH_16BPP                    0x2
#define NV5_PFB_CONFIG_0_DEPTH_32BPP                    0x3

#define NV5_PFB_CONFIG_1                                0x100204    // Framebuffer interface config register 1
#define NV5_PFB_END                                     0x100FFF
#define NV5_PEXTDEV_START                               0x101000    // External Devices
#define NV5_PSTRAPS                                     0x101000    // Straps Bits
#define NV5_PSTRAPS_BUS_SPEED                           0           // Configured bus speed
#define NV5_PSTRAPS_BUS_SPEED_33MHZ                     0x0
#define NV5_PSTRAPS_BUS_SPEED_66MHZ                     0x1
#define NV5_PSTRAPS_BIOS                                1           // Is a VBIOS present?
#define NV5_PSTRAPS_BIOS_NOT_PRESENT                    0x0
#define NV5_PSTRAPS_BIOS_PRESENT                        1
#define NV5_PSTRAPS_RAM_TYPE                            2           // Type of RAM module
#define NV5_PSTRAPS_RAM_TYPE_16MBIT                     0x0
#define NV5_PSTRAPS_RAM_TYPE_8MBIT                      0x1
#define NV5_PSTRAPS_NEC_MODE                            3           // PC98?
#define NV5_PSTRAPS_NEC_MODE_DISABLED                   0x0
#define NV5_PSTRAPS_NEC_MODE_ENABLED                    0x1
#define NV5_PSTRAPS_BUS_WIDTH                           4           // Bus width
#define NV5_PSTRAPS_BUS_WIDTH_64BIT                     0x0
#define NV5_PSTRAPS_BUS_WIDTH_128BIT                    0x0
#define NV5_PSTRAPS_BUS_TYPE                            5           // Determines if this is a PCI or AGP card
#define NV5_PSTRAPS_BUS_TYPE_PCI                        0x0
#define NV5_PSTRAPS_BUS_TYPE_AGP                        0x1
#define NV5_PSTRAPS_CRYSTAL                             6           // type of clock crystal
#define NV5_PSTRAPS_CRYSTAL_13500K                      0x0         // 13.5 Mhz
#define NV5_PSTRAPS_CRYSTAL_14318180                    0x1         // 14.318180 Mhz clock crystal
#define NV5_PSTRAPS_TVMODE                              7           // Type of TV signal to put out
#define NV5_PSTRAPS_TVMODE_SECAM                        0x0  
#define NV5_PSTRAPS_TVMODE_NTSC                         0x1
#define NV5_PSTRAPS_TVMODE_PAL                          0x2
#define NV5_PSTRAPS_TVMODE_NONE                         0x3
#define NV5_PSTRAPS_AGP2X                               9
#define NV5_PSTRAPS_AGP2X_ENABLED                       0x0
#define NV5_PSTRAPS_AGP2X_DISABLED                      0x1
#define NV5_PSTRAPS_UNUSED                              10
#define NV5_PSTRAPS_OVERWRITE                           11
#define NV5_PSTRAPS_OVERWRITE_DISABLED                  0x0
#define NV5_PSTRAPS_OVERWRITE_ENABLED                   0x1
#define NV5_PEXTDEV_END                                 0x101FFF
#define NV5_PROM_START                                  0x110000    // VBIOS?
#define NV5_PROM_END                                    0x11FFFF
#define NV5_PALT_START                                  0x120000    // ??? but it exists
#define NV5_PALT_END                                    0x12FFFF
#define NV5_PME_START                                   0x200000    // Mediaport 
#define NV5_PME_INTR                                    0x200100    // Mediaport: Interrupt Pending?
#define NV5_PME_INTR_EN                                 0x200140    // Mediaport: Interrupt Enable
#define NV5_PME_END                                     0x200FFF
#define NV5_PGRAPH_START                                0x400000    // Scene graph for 2d/3d rendering...the most important part
// PGRAPH Core
#define NV5_PGRAPH_DEBUG_0                              0x400080
#define NV5_PGRAPH_DEBUG_1                              0x400084
#define NV5_PGRAPH_DEBUG_2                              0x400088
#define NV5_PGRAPH_DEBUG_3                              0x40008C
#define NV5_PGRAPH_INTR_0                               0x400100
#define NV5_PGRAPH_INTR_1                               0x400104
#define NV5_PGRAPH_INTR_EN_0                            0x400140    // Interrupt Control for PGRAPH #1
#define NV5_PGRAPH_INTR_EN_0_VBLANK                     8           // Fired every frame
#define NV5_PGRAPH_INTR_EN_0_VBLANK_ENABLED             0x1         // Is the vblank interrupt enabled?
//todo: add what this does
#define NV5_PGRAPH_INTR_EN_1                            0x400144    // Interrupt Control for PGRAPH #2 (it can receive two at onc)
#define NV5_PGRAPH_CONTEXT_SWITCH                       0x400180    // DMA context switcher
#define NV5_PGRAPH_CONTEXT_CONTROL                      0x400190    // DMA context control
#define NV5_PGRAPH_CONTEXT_USER                         0x400194    // Current DMA context state, may rename
#define NV5_PGRAPH_CONTEXT_CACHE(i)                     0x4001A0+(i*4)  // Context Cache
#define NV5_PGRAPH_CONTEXT_CACHE_SIZE                   8
// TODO: CLIP0/CLIP1 (8 clips min/max in 32bits)
#define NV5_PGRAPH_ABS_UCLIP_XMIN                       0x40053C    // Clip X minimum
#define NV5_PGRAPH_ABS_UCLIP_XMAX                       0x400540    // Clip X maximum
#define NV5_PGRAPH_ABS_UCLIP_YMIN                       0x400544    // Clip Y minimum
#define NV5_PGRAPH_ABS_UCLIP_YMAX                       0x400548    // Clip Y maximum
#define NV5_PGRAPH_SRC_CANVAS_MIN                       0x400550    // Minimum Source Canvas for Blit, Y=30:16, X=10:0
#define NV5_PGRAPH_SRC_CANVAS_MAX                       0x400554    // Maximum Source Canvas for Blit, Y=30:16, X=10:0
#define NV5_PGRAPH_DST_CANVAS_MIN                       0x400558    // Minimum Destination Canvas for Blit, Y=30:16, X=10:0
#define NV5_PGRAPH_DST_CANVAS_MAX                       0x40055C    // Maximum Destination Canvas for Blit, Y=30:16, X=10:0
#define NV5_PGRAPH_PATTERN_COLOR_0_0                    0x400600    
#define NV5_PGRAPH_PATTERN_COLOR_0_1                    0x400604
#define NV5_PGRAPH_PATTERN_COLOR_1_0                    0x400608    
#define NV5_PGRAPH_PATTERN_COLOR_1_1                    0x40060C    // pattern color 
#define NV5_PGRAPH_PATTERN_BITMAP_HIGH                  0x400610    // pattern bitmap [31:0]
#define NV5_PGRAPH_PATTERN_BITMAP_LOW                   0x400614    // pattern bitmap [63:32]
#define NV5_PGRAPH_PATTERN_SHAPE                        0x400618
#define NV5_PGRAPH_ROP3                                 0x400624    // ROP3      
#define NV5_PGRAPH_PLANE_MASK                           0x400628
#define NV5_PGRAPH_CHROMA_KEY                           0x40062C
#define NV5_PGRAPH_BETA                                 0x400640    // Beta factor (30:23 fractional, 22:0 before fraction)
#define NV5_PGRAPH_DMA                                  0x400680
#define NV5_PGRAPH_NOTIFY                               0x400684    // Notifier for PGRAPH      
#define NV5_PGRAPH_CLIP0_MIN                            0x400690    // Clip for Blitting 0 Min
#define NV5_PGRAPH_CLIP0_MAX                            0x400694    // Clip for Blitting 0 Max
#define NV5_PGRAPH_CLIP1_MIN                            0x400698    // Clip for Blitting 1 Min
#define NV5_PGRAPH_CLIP1_MAX                            0x40069C    // Clip for Blitting 1 Max
#define NV5_PGRAPH_CLIP_MISC                            0x4006A0    // Regions/Render/Complex mode
#define NV5_PGRAPH_FIFO_ACCESS                          0x4006A4    // Is PGRAPH enabled?
#define NV5_PGRAPH_FIFO_ACCESS_DISABLED                 0x0
#define NV5_PGRAPH_FIFO_ACCESS_ENABLED                  0x1
#define NV5_PGRAPH_CLIP_MISC                            0x4006A0    // Miscellaneous clipping information
#define NV5_PGRAPH_STATUS                               0x4006B0    // Current PGRAPH status
#define NV5_PGRAPH_TRAPPED_ADDRESS                      0x4006B4
#define NV5_PGRAPH_TRAPPED_DATA                         0x4006B8
#define NV5_PGRAPH_TRAPPED_INSTANCE                     0x4006BC

#define NV5_PGRAPH_DMA_INTR_0                           0x401000    // PGRAPH DMA Interrupt Status
#define NV5_PGRAPH_DMA_INTR_EN_0                        0x401140    // PGRAPH DMA Interrupt Enable 0

// not sure about the class ids
// these are NOT what each class is, just uSed to manipulate it (there isn't a one to one class->reg mapping anyway)
#define NV5_PGRAPH_CLASS18_BETA_START                   0x410000    // Beta blending factor
#define NV5_PGRAPH_CLASS18_BETA_END                     0x411FFF  
#define NV5_PGRAPH_CLASS20_ROP_START                    0x420000    // Blending render operation used at final pixel/fragment generation stage
#define NV5_PGRAPH_CLASS20_ROP_END                      0x421FFF
#define NV5_PGRAPH_CLASS21_COLORKEY_START               0x430000    // Color key for image
#define NV5_PGRAPH_CLASS21_COLORKEY_END                 0x431FFF      
#define NV5_PGRAPH_CLASS22_PLANEMASK_START              0x440000    // Plane mask (for clipping?)
#define NV5_PGRAPH_CLASS22_PLANEMASK_END                0x441FFF
#define NV5_PGRAPH_CLASSXX_CLIP_START                   0x450000    // clipping, probably class 23
#define NV5_PGRAPH_CLASSXX_CLIP_END                     0x451FFF
#define NV5_PGRAPH_CLASS24_PATTERN_START                0x460000    // presumably a blend pattern 
#define NV5_PGRAPH_CLASS24_PATTERN_END                  0x461FFF
#define NV5_PGRAPH_CLASS30_RECTANGLE_START              0x470000    // also class 25 - that's black [NV1]
#define NV5_PGRAPH_CLASS30_RECTANGLE_END                0x471FFF    // also class 25 - that's black [NV1]
#define NV5_PGRAPH_CLASS26_POINT_START                  0x480000    // A single point
#define NV5_PGRAPH_CLASS26_POINT_END                    0x481FFF
#define NV5_PGRAPH_CLASS27_LINE_START                   0x490000    // A line
#define NV5_PGRAPH_CLASS27_LINE_END                     0x491FFF       
#define NV5_PGRAPH_CLASS28_LIN_START                    0x4A0000    // A lin - a line without its starting or ending pixels
#define NV5_PGRAPH_CLASS28_LIN_END                      0x4A1FFF
#define NV5_PGRAPH_CLASS29_TRIANGLE_START               0x4B0000    // A triangle [NV1 variant] - in NV1 this was converted to a quad patch
#define NV5_PGRAPH_CLASS29_TRIANGLE_END                 0x4B1FFF   
#define NV5_PGRAPH_CLASS75_GDITEXT_START                0x4C0000    // Windows 95/NT GDI text acceleration
#define NV5_PGRAPH_CLASS75_GDITEXT_END                  0x4C1FFF    

#define NV5_PGRAPH_CLASS61_MEM2MEM_XFER_START           0x4D0000    // memory to memory transfer (not sure about which class this is)
#define NV5_PGRAPH_CLASS61_MEM2MEM_XFER_END             0x4D1FFF    
#define NV5_PGRAPH_CLASSXX_IMAGE2MEM_XFER_SCALED_START  0x4E0000    // class 55, 56
#define NV5_PGRAPH_CLASSXX_IMAGE2MEM_XFER_SCALED_END    0x4E1FFF

#define NV5_PGRAPH_CLASS31_BLIT_START                   0x500000    // Blit 2d image from memory
#define NV5_PGRAPH_CLASS31_BLIT_END                     0x501FFF      

#define NV5_PGRAPH_CLASSXX_CPU2MEM_IMAGE_START          0x510000    // Used for class 33, 34, 54
#define NV5_PGRAPH_CLASSXX_CPU2MEM_IMAGE_END            0x511FFF    
#define NV5_PGRAPH_CLASSXX_CPU2MEM_BITMAP_START         0x520000    // not sure, might depend on format
#define NV5_PGRAPH_CLASSXX_CPU2MEM_BITMAP_END           0x521FFF    

#define NV5_PGRAPH_CLASSXX_IMAGE2MEM_XFER_START         0x540000    // send image to vram, not sure what class 
#define NV5_PGRAPH_CLASSXX_IMAGE2MEM_XFER_END           0x541FFF    
#define NV5_PGRAPH_CLASS54_CPU2MEM_STRETCHED_START      0x550000    // stretched cpu->vram transfer, 54
#define NV5_PGRAPH_CLASS54_CPU2MEM_STRETCHED_END        0x551FFF  

#define NV5_PGRAPH_CLASS72_D3D5TRI_ZETA_START           0x570000    // [NV3] Copy a direct3d 5.0 accelerated triangle to the zeta buffer
#define NV5_PGRAPH_CLASS72_D3D5TRI_ZETA_END             0x571FFF    
#define NV5_PGRAPH_CLASSXX_POINTZETA_START              0x580000    // possibly class 69
#define NV5_PGRAPH_CLASSXX_POINTZETA_END                0x581FFF

#define NV5_PGRAPH_CLASS62_MEM2IMAGE_START              0x5C0000    // class 55, 56, 62, 63?
#define NV5_PGRAPH_CLASS62_MEM2IMAGE_END                0x5C1FFF    

#define NV5_PGRAPH_REGISTER_END                         0x401FFF    // end of pgraph registers
#define NV5_PGRAPH_REAL_END                             0x5C1FFF

#define NV5_PRMCIO_START                                0x601000
// Following four are CRTC+I2C access registers
// and get redirected to VGA
#define NV5_PRMCIO_CRTC_REGISTER_CUR_INDEX_MONO         0x6013B4    // Current CRTC Register Index - Monochrome
#define NV5_PRMCIO_CRTC_REGISTER_CUR_MONO               0x6013B5    // Currently Selected CRTC Register - Monochrome
#define NV5_PRMCIO_CRTC_REGISTER_CUR_INDEX_COLOR        0x6013D4    // Current CRTC Register Index - Colour
#define NV5_PRMCIO_CRTC_REGISTER_CUR_COLOR              0x6013D5    
#define NV5_PRMCIO_END                                  0x601FFF

#define NV5_PDAC_START                                  0x680000    // OPTIONAL external DAC
#define NV5_PVIDEO_START                                0x680000    // Video Generation / overlay configuration
#define NV5_PVIDEO_INTR                                 0x680100
#define NV5_PVIDEO_INTR_EN                              0x680140
#define NV5_PVIDEO_END                                  0x6802FF
#define NV5_PRAMDAC_START                               0x680300

#define NV5_PRAMDAC_CLOCK_MEMORY                        0x680504
#define NV5_PRAMDAC_CLOCK_MEMORY_VDIV                   7:0
#define NV5_PRAMDAC_CLOCK_MEMORY_NDIV                   15:8
#define NV5_PRAMDAC_CLOCK_MEMORY_PDIV                   18:16
#define NV5_PRAMDAC_CLOCK_PIXEL                         0x680508
#define NV5_PRAMDAC_COEFF_SELECT                        0x68050C

#define NV5_PRAMDAC_GENERAL_CONTROL                     0x680600

// These are all 10-bit values, but aligned to 32bits
// so treating them as 32bit should be fine
#define NV5_PRAMDAC_VSERR_WIDTH                         0x680700
#define NV5_PRAMDAC_VEQU_END                            0x680704
#define NV5_PRAMDAC_VBBLANK_END                         0x680708
#define NV5_PRAMDAC_VBLANK_END                          0x68070C
#define NV5_PRAMDAC_VBLANK_START                        0x680710
#define NV5_PRAMDAC_VBBLANK_START                       0x680714
#define NV5_PRAMDAC_VEQU_START                          0x680718
#define NV5_PRAMDAC_VTOTAL                              0x68071C
#define NV5_PRAMDAC_HSYNC_WIDTH                         0x680720
#define NV5_PRAMDAC_HBURST_START                        0x680724
#define NV5_PRAMDAC_HBURST_END                          0x680728
#define NV5_PRAMDAC_HBLANK_START                        0x68072C
#define NV5_PRAMDAC_HBLANK_END                          0x680730
#define NV5_PRAMDAC_HTOTAL                              0x680734
#define NV5_PRAMDAC_HEQU_WIDTH                          0x680738
#define NV5_PRAMDAC_HSERR_WIDTH                         0x68073C

#define NV5_PRAMDAC_END                                 0x680FFF
#define NV5_PDAC_END                                    0x680FFF    // OPTIONAL external DAC


#define NV5_USER_START                                  0x800000    // Mapping for the area where objects are submitted into the FIFO (up to 0x880000?)
#define NV5_USER_END                                    0xFFFFFF

// easier name
#define NV5_OBJECT_SUBMIT_START                         NV5_USER_START
#define NV5_OBJECT_SUBMIT_END                           NV5_USER_END

// also PDFB (Debug Framebuffer?)
#define NV5_PNVM_START                                  0x1000000   // VRAM access (max 8MB)
#define NV5_PNVM_END                                    0x17FFFFF   

// to be less confusing - NVM = "NV Memory"
#define NV5_VRAM_START                                  NV5_PNVM_START
#define NV5_VRAM_END                                    NV5_PNVM_END

// control structures for dma'd in graphics objects from pfifo
// these all have configurable sizes, define them here
#define NV5_PRAMIN_START                                0x1C00000



#define NV5_PRAMIN_RAMHT_START                          0x1C00000   // Hashtable for storing submitted objects
#define NV5_PRAMIN_RAMHT_END                            0x1C00FFF
#define NV5_PRAMIN_RAMHT_SIZE_0                         0xFFF
#define NV5_PRAMIN_RAMHT_SIZE_1                         0x1FFF
#define NV5_PRAMIN_RAMHT_SIZE_2                         0x3FFF
#define NV5_PRAMIN_RAMHT_SIZE_3                         0x7FFF

#define NV5_PRAMIN_RAMAU_START                          0x1C01000   // Auxillary area
#define NV5_PRAMIN_RAMAU_END                            0x1C01BFF
#define NV5_PRAMIN_RAMFC_START                          0x1C01C00   // context for unused PFIFO DMA channels
#define NV5_PRAMIN_RAMFC_END                            0x1C01DFF
#define NV5_PRAMIN_RAMFC_SIZE_0                         0x1FF
#define NV5_PRAMIN_RAMFC_SIZE_1                         0xFFF
#define NV5_PRAMIN_RAMRO_START                          0x1C01E00   // Runout area for invalid submissions
#define NV5_PRAMIN_RAMRO_SIZE_0                         0x1FF
#define NV5_PRAMIN_RAMRO_SIZE_1                         0x1FFF
#define NV5_PRAMIN_RAMRO_END                            0x1C01FFF
#define NV5_PRAMIN_RAMRM_START                          0x1C02000
#define NV5_PRAMIN_RAMRM_END                            0x1C02FFF

#define NV5_PRAMIN_END                                  0x1FFFFFF

// not done

// Master Control


// CRTC/CIO (0x3b0-0x3df)

#define NV5_CRTC_DATA_OUT                               0x3C0
#define NV5_CRTC_MISCOUT                                0x3C2

// These are standard (0-18h)
#define NV5_CRTC_REGISTER_HTOTAL                        0x00
#define NV5_CRTC_REGISTER_HDISPEND                      0x01
#define NV5_CRTC_REGISTER_HBLANKSTART                   0x02
#define NV5_CRTC_REGISTER_HBLANKEND                     0x03
#define NV5_CRTC_REGISTER_HRETRACESTART                 0x04
#define NV5_CRTC_REGISTER_HRETRACEEND                   0x05
#define NV5_CRTC_REGISTER_VTOTAL                        0x06
#define NV5_CRTC_REGISTER_OVERFLOW                      0x07
#define NV5_CRTC_REGISTER_PRESETROWSCAN                 0x08
#define NV5_CRTC_REGISTER_MAXSCAN                       0x09
#define NV5_CRTC_REGISTER_CURSOR_START                  0x0A
#define NV5_CRTC_REGISTER_CURSOR_END                    0x0B
#define NV5_CRTC_REGISTER_STARTADDR_HIGH                0x0C
#define NV5_CRTC_REGISTER_STARTADDR_LOW                 0x0D
#define NV5_CRTC_REGISTER_CURSORLOCATION_HIGH           0x0E
#define NV5_CRTC_REGISTER_CURSORLOCATION_LOW            0x0F
#define NV5_CRTC_REGISTER_VRETRACESTART                 0x10
#define NV5_CRTC_REGISTER_VRETRACEEND                   0x11
#define NV5_CRTC_REGISTER_VDISPEND                      0x12
#define NV5_CRTC_REGISTER_OFFSET                        0x13
#define NV5_CRTC_REGISTER_UNDERLINELOCATION             0x14
#define NV5_CRTC_REGISTER_STARTVBLANK                   0x15
#define NV5_CRTC_REGISTER_ENDVBLANK                     0x16
#define NV5_CRTC_REGISTER_CRTCCONTROL                   0x17
#define NV5_CRTC_REGISTER_LINECOMP                      0x18
#define NV5_CRTC_REGISTER_STANDARDVGA_END               0x18


// These are nvidia, licensed from weitek (25-63)
#define NV5_CRTC_REGISTER_RPC0                          0x19        // What does this mean?
#define NV5_CRTC_REGISTER_RPC1                          0x1A        // What does this mean?
#define NV5_CRTC_REGISTER_READ_BANK                     0x1D
#define NV5_CRTC_REGISTER_WRITE_BANK                    0x1E
#define NV5_CRTC_REGISTER_FORMAT                        0x25
#define NV5_CRTC_REGISTER_FORMAT_VDT10                  0           // Use 10 bit vtotal value instead of 8 bit
#define NV5_CRTC_REGISTER_FORMAT_VDE10                  1           // Use 10 bit dispend value instead of 8 bit
#define NV5_CRTC_REGISTER_FORMAT_VRS10                  2           // Use 10 bit vblank start value instead of 8 bit
#define NV5_CRTC_REGISTER_FORMAT_VBS10                  3           // Use 10 bit vsync start value instead of 8 bit
#define NV5_CRTC_REGISTER_FORMAT_HBE6                   4           // Use 6 bit hsync start value
#define NV5_CRTC_REGISTER_PIXELMODE                     0x28

#define NV5_CRTC_REGISTER_HEB                           0x2D        // HRS most significant bit

#define NV5_CRTC_REGISTER_PIXELMODE_VGA                 0x00        // vga textmode
#define NV5_CRTC_REGISTER_PIXELMODE_8BPP                0x01
#define NV5_CRTC_REGISTER_PIXELMODE_16BPP               0x02
#define NV5_CRTC_REGISTER_PIXELMODE_32BPP               0x03 

#define NV5_CRTC_REGISTER_RL0                           0x34
#define NV5_CRTC_REGISTER_RL1                           0x35
#define NV5_CRTC_REGISTER_RMA                           0x38        // REAL MODE ACCESS!
#define NV5_CRTC_REGISTER_I2C                           0x3E    
#define NV5_CRTC_REGISTER_I2C_GPIO                      0x3F

// where the fuck is GDC?
#define NV5_CRTC_BANKED_128K_A0000                      0x00
#define NV5_CRTC_BANKED_64K_A0000                       0x04
#define NV5_CRTC_BANKED_32K_B0000                       0x08
#define NV5_CRTC_BANKED_32K_B8000                       0x0C


#define NV5_RMA_REGISTER_START                          0x3D0
#define NV5_RMA_REGISTER_END                            0x3D3

#define NV5_CRTC_REGISTER_NVIDIA_END                    0x3F
// for 86box 8bit addressing
// get rid of this asap, replace with 32->8 macros
#define NV5_RMA_SIGNATURE_MSB                           0x65
#define NV5_RMA_SIGNATURE_BYTE2                         0xD0
#define NV5_RMA_SIGNATURE_BYTE1                         0x16
#define NV5_RMA_SIGNATURE_LSB                           0x2B

#define NV5_CRTC_REGISTER_RMA_MODE_MAX                  0x0F


/* 
    STRUCTURES FOR THE GPU START HERE
    OBJECT CLASS & RENDERING RELATED STUFF IS IN VID_NV5_CLASSES.H
*/


//todo: pixel format

// Master Control
typedef struct nv5_pmc_s
{
    /* 
    Holds chip manufacturing information at bootup.
    Current specification (may change later): pre-packed for convenience

    FIB Revision 1, Mask Revision B0, Implementation 1 [NV3], Architecture 3 [NV3], Manufacturer Nvidia, Foundry SGS (seems to have been the most common?)
    31:28=0000, 27:24=0000, 23:16=0003, 15:8=0001, 7:4=0001, 3:0=0001
    little endian 00000000 00000011 00000001 00010001   = 0x00300111
    */
    int32_t boot;   
    int32_t interrupt_status;          // Determines if interrupts are pending for specific subsystems.
    int32_t interrupt_enable;   // Determines if interrupts are actually enabled.
    int32_t enable;             // Determines which subsystems are enabled.

} nv5_pmc_t;

typedef struct nv5_pci_config_s
{
    uint8_t pci_regs[NV_PCI_NUM_CFG_REGS];  // The actual pci register values (not really used, just so they can be stored - they aren't very good for code readability)
    bool    vbios_enabled;                  // is the vbios enabled?
    uint8_t int_line;
} nv5_pci_config_t;

/* Notifier Engine */
typedef struct nv5_notifier_s
{
    /* TODO */
} nv5_notifier_t;

// add enums for eac
// Chip configuration
typedef struct nv5_straps_s
{
    uint32_t straps;
} nv5_straps_t;

// Framebuffer
typedef struct nv5_pfb_s
{
    uint32_t boot;
    uint32_t config_0;
    uint32_t config_1;
} nv5_pfb_t;

#define NV5_RMA_NUM_REGS        4
// Access the GPU from real-mode
typedef struct nv5_pbus_rma_s
{
    uint32_t addr;                      // Address to RMA to
    uint32_t data;                      // Data to send to MMIO
    uint8_t mode;                       // the current state of the rma shifting engin
    uint8_t rma_regs[NV5_RMA_NUM_REGS]; // The rma registers (saved)
} nv5_pbus_rma_t;

// Bus Configuration
typedef struct nv5_pbus_s
{
    uint32_t interrupt_status;          // Interrupt status
    uint32_t interrupt_enable;          // Interrupt enable
    nv5_pbus_rma_t rma;
} nv5_pbus_t;

// Command submission to PGRAPH
typedef struct nv5_pfifo_s
{
    uint32_t interrupt_status;          // Interrupt status
    uint32_t interrupt_enable;          // Interrupt enable
    uint32_t ramht_config;              // RAMHT config
    uint32_t ramfc_config;              // RAMFC config
    uint32_t ramro_config;              // RAMRO config
    uint32_t cache_reassignment;        // Enable automatic reassignment into CACHE0?

} nv5_pfifo_t;

// create_object(uint32_t type) here

// RAMDAC
typedef struct nv5_pramdac_s
{
    // these should be uint8_t but C math is a lot better with this
    uint32_t memory_clock_m;    // memory clock M-divider
    uint32_t memory_clock_n;    // memory clock N-divider
    uint32_t memory_clock_p;    // memory clock P-divider
    uint32_t pixel_clock_m;     // pixel clock M-divider
    uint32_t pixel_clock_n;     // pixel clock N-divider
    uint32_t pixel_clock_p;     // pixel clock P-divider
    uint32_t coeff_select;      // coefficient select

    uint32_t general_control;   // general control register

    // this could duplicate SVGA state but I tihnk it's more readable,
    // we'll just modify both
    uint32_t vserr_width;       // vertical sync error width
    uint32_t vequ_end;          // vequ end (not sure what this is)
    uint32_t vbblank_end;       // vbblank end (not sure what this is)
    uint32_t vblank_end;        // vblank end
    uint32_t vblank_start;      // vblank start
    uint32_t vequ_start;        // vequ start (not sure what this is)
    uint32_t vtotal;            // vertical total lines
    uint32_t hsync_width;       // horizontal sync width
    uint32_t hburst_start;      // horizontal burst signal start (in lines)
    uint32_t hburst_end;        // horizontal burst signal end (in lines)
    uint32_t hblank_start;      // horizontal blank start (in lines)
    uint32_t hblank_end;        // horizontal blank end (in lines)
    uint32_t htotal;            // horizontal total lines
    uint32_t hequ_width;        // horizontal equ width (not sure what this is)
    uint32_t hserr_width;       // horizontal sync error width
} nv5_pramdac_t;

/* Holds DMA context channel information */
typedef struct nv5_pgraph_context_switch_s
{
    /* TODO */
} nv5_pgraph_context_switch_t;

typedef struct nv5_pgraph_context_control_s
{
    /* TODO */
} nv5_pgraph_context_control_t;

/* DMA object context info 
   Context uploaded from CACHE0/CACH1 by DMA Puller
*/
typedef struct nv5_pgraph_context_user_s
{
    bool reserved3 : 1;
    uint8_t channel : 7;
    uint8_t reserved2 : 4;
    uint8_t class : 5;
    uint8_t subchannel : 3;
    uint16_t reserved : 12;
} nv5_pgraph_context_user_t; 

typedef struct nv5_pgraph_dma_settings_s
{
    /* TODO */
} nv5_pgraph_dma_settings_t;

typedef struct nv5_pgraph_clip_misc_settings_s
{
    /* TODO */
} nv5_pgraph_clip_misc_settings_t;

// Graphics Subsystem
typedef struct nv5_pgraph_s
{
    uint32_t debug_0;
    uint32_t debug_1;
    uint32_t debug_2;
    uint32_t debug_3;
    uint32_t interrupt_status_0;          // Interrupt status 0
    uint32_t interrupt_enable_0;          // Interrupt enable 0 
    uint32_t interrupt_status_1;          // Interrupt status 1
    uint32_t interrupt_enable_1;          // Interrupt enable 1

    nv5_pgraph_context_control_t context_control;
    nv5_pgraph_context_switch_t context_user_submit;
    nv5_pgraph_context_user_t context_user;
    uint32_t context_cache[NV5_PGRAPH_CONTEXT_CACHE_SIZE];  // DMA context cache (nv5_pgraph_context_user_t array?)

    // UCLIP stuff
    uint32_t abs_uclip_xmin;
    uint32_t abs_uclip_xmax;
    uint32_t abs_uclip_ymin;
    uint32_t abs_uclip_ymax;
    nv5_position_16_bigy_t src_canvas_min;
    nv5_position_16_bigy_t src_canvas_max;
    nv5_position_16_bigy_t dst_canvas_min;
    nv5_position_16_bigy_t dst_canvas_max;
    nv5_color_x3a10g10b10_t pattern_color_0_0;
    uint32_t pattern_color_0_1;                             // only 7:0 relevant
    nv5_color_x3a10g10b10_t pattern_color_1_0;
    uint32_t pattern_color_1_1;                             // only 7:0 relevant
    uint32_t pattern_bitmap_high;                           // high part of pattern bitmap for blit
    uint32_t pattern_bitmap_low;
    uint32_t pattern_shape;                                 // may need to be an enum - 0=8x8, 1=64x1, 2=1x64
    uint32_t plane_mask;                                    // only 7:0 relevant
    nv5_color_x3a10g10b10_t chroma_key;                     // color key
    uint32_t beta_factor;
    nv5_pgraph_dma_settings_t dma_settings;
    nv5_pgraph_clip_misc_settings_t clip_misc_settings;
    nv5_notifier_t notifier;
} nv5_pgraph_t;

// GPU Manufacturing Configuration (again)
// In the future this will be configurable
typedef struct nv5_pextdev_s
{
    /*
    // Disabled     33Mhz
    // Enabled      66Mhz
    bool bus_speed;
    
    // Disabled     No BIOS
    // Enabled      BIOS
    bool bios;

    // RAM Type #1
    // Disabled     16Mbit (2MB) module size
    // Enabled      8Mbit (1MB) module size
    bool ram_type_1;

    // NEC Mode
    bool nec_mode;

    // Disabled     64-bit
    // Enabled      128-bit
    bool bus_width;

    // Disabled     PCI
    // Enabled      AGP
    bool bus_type;

    // Disabled     13500
    // Enabled      14318180
    bool crystal;

    // TV Mode
    // 0 - SECAM, 1 - NTSC, 2 - PAL, 3 - none
    uint8_t tv_mode;

    // AGP 2X mode
    // Disabled     AGP 1X
    // Enabled      AGP 2X
    bool agp_is_2x; 
    
    bool unused;

    // Overwrite enable
    bool overwrite;

    See defines in vid_nv3.h
    */
    uint32_t straps;
    
    // more ram type stuff here but not used?
} nv5_pextdev_t;

typedef struct nv5_ptimer_s
{
    uint32_t interrupt_status;          // PTIMER Interrupt status
    uint32_t interrupt_enable;          // PTIMER Interrupt enable
    uint32_t clock_numerator;           // PTIMER (tick?) numerator
    uint32_t clock_denominator;         // PTIMER (tick?) denominator
    uint64_t time;                      // time
    uint32_t alarm;                     // The value of time when there should be an alarm
} nv5_ptimer_t;

typedef struct nv5_pramin_name_sd
{
    union 
    {
        uint32_t name;
        uint8_t byte_high;
        uint8_t byte_mid2;
        uint8_t byte_mid1;
        uint8_t byte_low;
    };
} nv5_pramin_name_t;

typedef struct nv5_pramin_context_s
{
    union 
    {
        uint32_t context; 
        uint8_t dma_channel;
        uint8_t render_object;              //0=sw, 1=hw accelerated render
        uint8_t class_id;
        uint8_t ramin_offset;               //find
    };
} nv5_pramin_context_t;

// Graphics object hashtable for specific DMA [channel, subchannel] pair
typedef struct nv5_pramin_ramht_subchannel_s
{
    nv5_pramin_name_t    name;                      // must be >4096

    // Contextual information.
    // See the above union.
    nv5_pramin_context_t context;                       
} nv5_pramin_ramht_subchannel_t;

// Graphics object hashtable
typedef struct nv5_pramin_ramht_s
{
    nv5_pramin_ramht_subchannel_t subchannels[NV5_DMA_CHANNELS][NV5_DMA_SUBCHANNELS_PER_CHANNEL];
} nv5_pramin_ramht_t;

uint32_t nv5_ramht_hash(nv5_pramin_name_t name, uint32_t channel);

typedef enum nv5_pramin_ramro_reason_e
{
    nv5_runout_reason_illegal_access = 0,

    // PFIFO CACHE0/CACHE1 were turned off, so the graphics object could not be processed.
    nv5_runout_reason_no_cache_available = 1,

    // Ran out of CACHE0 & CACHE1 space.
    nv5_runout_reason_cache_ran_out = 2,

    nv5_runout_reason_free_count_overrun = 3,

    nv5_runout_reason_caught_lying = 4,

    // Access reserved by pagetable
    nv5_runout_reason_reserved_access = 5,

} nv5_pramin_ramro_reason;

/* This is a gigantic error handling system */
typedef struct nv5_pramin_ramro_entry_s
{
    
    //todo
} nv5_pramin_ramro_entry_t;

// Anti-fuckup device
typedef struct nv5_pramin_ramro_s
{

} nv5_pramin_ramro_t;

// context for unused channels
typedef struct nv5_pramin_ramfc_s
{

} nv5_pramin_ramfc_t;

// ????? ram auxillary
typedef struct nv_pramin_ramau_s
{

} nv5_pramin_ramau_t;

typedef struct nv5_pramin_s
{

} nv5_pramin_t;

typedef struct nv5_pvideo_s
{
    uint32_t interrupt_status;          // Interrupt status
    uint32_t interrupt_enable;          // Interrupt enable
} nv5_pvideo_t;

typedef struct nv5_pme_s                // Mediaport
{
    uint32_t interrupt_status;
    uint32_t interrupt_enable;
} nv5_pme_t;

typedef struct nv5_s
{
    nv_base_t nvbase;   // Base Nvidia structure
    
    // Config
    nv5_straps_t straps;
    nv5_pci_config_t pci_config;

    // Subsystems
    nv5_pmc_t pmc;              // Master Control
    nv5_pfb_t pfb;              // Framebuffer/VRAM
    nv5_pbus_t pbus;            // Bus Control
    nv5_pfifo_t pfifo;          // FIFO for command submisison

    nv5_pramdac_t pramdac;      // RAMDAC (CLUT etc)
    nv5_pgraph_t pgraph;        // 2D/3D Graphics
    nv5_pextdev_t pextdev;      // Chip configuration
    nv5_ptimer_t ptimer;        // programmable interval timer
    nv5_pramin_ramht_t ramht;   // hashtable for PGRAPH objects
    nv5_pramin_ramro_t ramro;   // anti-fuckup mechanism for idiots who fucked up the FIFO submission
    nv5_pramin_ramfc_t ramfc;   // context for unused channels
    nv5_pramin_ramau_t ramau;   // auxillary weirdnes
    nv5_pramin_t pramin;        // Ram for INput of DMA objects. Very important!
    nv5_pvideo_t pvideo;        // Video overlay
    nv5_pme_t pme;              // Mediaport - external MPEG decoder and video interface
    //more here

} nv5_t;

// device objects
extern nv5_t* nv5;

// NV5 stuff

// Device Core
void*       nv5_init(const device_t *info);
void        nv5_close(void* priv);
void        nv5_speed_changed(void *priv);
void        nv5_force_redraw(void* priv);

// Memory Mapping
void        nv5_update_mappings();                                              // Update memory mappings
uint8_t     nv5_mmio_read8(uint32_t addr, void* priv);                          // Read 8-bit MMIO
uint16_t    nv5_mmio_read16(uint32_t addr, void* priv);                         // Read 16-bit MMIO
uint32_t    nv5_mmio_read32(uint32_t addr, void* priv);                         // Read 32-bit MMIO
void        nv5_mmio_write8(uint32_t addr, uint8_t val, void* priv);            // Write 8-bit MMIO
void        nv5_mmio_write16(uint32_t addr, uint16_t val, void* priv);          // Write 16-bit MMIO
void        nv5_mmio_write32(uint32_t addr, uint32_t val, void* priv);          // Write 32-bit MMIO

uint8_t     nv5_svga_in(uint16_t addr, void* priv);                             // Read SVGA compatibility registers
void        nv5_svga_out(uint16_t addr, uint8_t val, void* priv);               // Write SVGA registers
uint8_t     nv5_pci_read(int32_t func, int32_t addr, void* priv);               // Read PCI configuration registers
void        nv5_pci_write(int32_t func, int32_t addr, uint8_t val, void* priv); // Write PCI configuration registers

uint8_t     nv5_ramin_read8(uint32_t addr, void* priv);                         // Read 8-bit RAMIN
uint16_t    nv5_ramin_read16(uint32_t addr, void* priv);                        // Read 16-bit RAMIN
uint32_t    nv5_ramin_read32(uint32_t addr, void* priv);                        // Read 32-bit RAMIN
void        nv5_ramin_write8(uint32_t addr, uint8_t val, void* priv);           // Write 8-bit RAMIN
void        nv5_ramin_write16(uint32_t addr, uint16_t val, void* priv);         // Write 16-bit RAMIN
void        nv5_ramin_write32(uint32_t addr, uint32_t val, void* priv);         // Write 32-bit RAMIN

bool        nv5_pramin_arbitrate_read(uint32_t address, uint32_t* value);       // Read arbitration so we can read/write to the structures in the first 64k of ramin
bool        nv5_pramin_arbitrate_write(uint32_t address, uint32_t value);       // Write arbitration so we can read/write to the structures in the first 64k of ramin

uint32_t    nv5_ramfc_read(uint32_t address);
void        nv5_ramfc_write(uint32_t address, uint32_t value);
uint32_t    nv5_ramro_read(uint32_t address);
void        nv5_ramro_write(uint32_t address, uint32_t value);
uint32_t    nv5_ramht_read(uint32_t address);
void        nv5_ramht_write(uint32_t address, uint32_t value);

// MMIO Arbitration
// Determine where the hell in this mess our reads or writes are going
uint32_t    nv5_mmio_arbitrate_read(uint32_t address);
void        nv5_mmio_arbitrate_write(uint32_t address, uint32_t value);

// Read and Write functions for GPU subsystems
// Remove the ones that aren't used here eventually, have all of htem for now
uint32_t    bc5_pmc_read(uint32_t address);
void        nv5_pmc_write(uint32_t address, uint32_t value);
uint32_t    nv5_cio_read(uint32_t address);
void        nv5_cio_write(uint32_t address, uint32_t value);
uint32_t    nv5_pbus_read(uint32_t address);
void        nv5_pbus_write(uint32_t address, uint32_t value);
uint32_t    nv5_pfifo_read(uint32_t address);
void        nv5_pfifo_write(uint32_t address, uint32_t value);
uint32_t    nv5_prm_read(uint32_t address);
void        nv5_prm_write(uint32_t address, uint32_t value);
uint32_t    nv5_prmio_read(uint32_t address);
void        nv5_prmio_write(uint32_t address, uint32_t value);
uint32_t    nv5_ptimer_read(uint32_t address);
void        nv5_ptimer_write(uint32_t address, uint32_t value);
uint32_t    nv5_pfb_read(uint32_t address);
void        nv5_pfb_write(uint32_t address, uint32_t value);
uint32_t    nv5_pextdev_read(uint32_t address);
void        nv5_pextdev_write(uint32_t address, uint32_t value);

// Special consideration for straps
#define nv5_pstraps_read nv5_pextdev_read(NV5_PSTRAPS)
#define nv5_pstraps_write(x) nv5_pextdev_write(NV5_PSTRAPS, x)

uint32_t    nv5_prom_read(uint32_t address);
void        nv5_prom_write(uint32_t address, uint32_t value);
uint32_t    nv5_palt_read(uint32_t address);
void        nv5_palt_write(uint32_t address, uint32_t value);
uint32_t    nv5_pme_read(uint32_t address);
void        nv5_pme_write(uint32_t address, uint32_t value);
uint32_t    nv5_pgraph_read(uint32_t address);
void        nv5_pgraph_write(uint32_t address, uint32_t value);

// TODO: PGRAPH class registers

uint32_t    nv5_prmcio_read(uint32_t address);
void        nv5_prmcio_write(uint32_t address, uint32_t value);
uint32_t    nv5_pvideo_read(uint32_t address);
void        nv5_pvideo_write(uint32_t address, uint32_t value);
uint32_t    nv5_pramdac_read(uint32_t address);
void        nv5_pramdac_write(uint32_t address, uint32_t value);
uint32_t    nv5_vram_read(uint32_t address);
void        nv5_vram_write(uint32_t address, uint32_t value);
#define nv5_nvm_read nv5_vram_read
#define nv5_nvm_write nv5_vram_write
uint32_t    nv5_user_read(uint32_t address);
void        nv5_user_write(uint32_t address, uint32_t value);
#define nv5_object_submit_start nv5_user_read
#define nv5_object_submit_end nv5_user_write
// TODO: RAMHT, RAMFC...or maybe handle it inside of nv5_pramin_*

// GPU subsystems

// NV5 PMC
void        nv5_pmc_init();
uint32_t    nv5_pmc_clear_interrupts();
uint32_t    nv5_pmc_handle_interrupts(bool send_now);

// NV5 PGRAPH
void        nv5_pgraph_init();
void        nv5_pgraph_vblank_start(svga_t* svga);

// NV5 PFIFO
void        nv5_pfifo_init();


// NV5 PFB
void        nv5_pfb_init();

// NV5 PEXTDEV/PSTRAPS
void        nv5_pextdev_init();

// NV5 PBUS
void        nv5_pbus_init();

// NV5 PBUS RMA - Real Mode Access for VBIOS
uint8_t     nv5_pbus_rma_read(uint16_t addr);
void        nv5_pbus_rma_write(uint16_t addr, uint8_t val);

// NV5 PRAMDAC (Final presentation)
void        nv5_pramdac_init();
void        nv5_pramdac_set_vram_clock();
void        nv5_pramdac_set_pixel_clock();
void        nv5_pramdac_pixel_clock_poll(double real_time);
void        nv5_pramdac_memory_clock_poll(double real_time);

// NV5 PTIMER
void        nv5_ptimer_init();
void        nv5_ptimer_tick(double real_time);

// NV5 PVIDEO
void        nv5_pvideo_init();

// NV5 PMEDIA (Mediaport)
void        nv5_pmedia_init();
