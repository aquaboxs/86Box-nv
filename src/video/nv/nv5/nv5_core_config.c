/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Provides NV5 configuration
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
#include <86box/io.h>
#include <86box/pci.h>
#include <86Box/rom.h> // DEPENDENT!!!
#include <86Box/video.h>
#include <86Box/nv/vid_nv.h>
#include <86Box/nv/vid_nv5.h>

const device_config_t nv5_config[] =
{
    // VBIOS type configuration
    {
        .name = "VBIOS",
        .description = "VBIOS",
        .type = CONFIG_BIOS,
        .default_string = "NV5_VBIOS_POWERCOLOR_CM64A",
        .default_int = 0,
        .bios = 
        {
           { 
                .name = "[NV5] nVidia RIVA TNT2 M64 (V2.05.20.02.80)", .files_no = 1,
                .internal_name = "NV5_VBIOS_UNKNOWN",
                .files = {NV5_VBIOS_UNKNOWN, ""} 
           },
           { 
                .name = "[NV5] ASUS AGP-V3800M (V3.05.00.10.31)", .files_no = 1,
                .internal_name = "NV5_VBIOS_ASUS_AGP_V3800M",
                .files = {NV5_VBIOS_ASUS_AGP_V3800M, ""} 
           },
           { 
                .name = "[NV5] Creative CT6984 AGP (V2.05.4.17.03)", .files_no = 1,
                .internal_name = "NV5_VBIOS_CREATIVE_CT6984",
                .files = {NV5_VBIOS_CREATIVE_CT6984, ""} 
           },
           { 
                .name = "[NV5] PowerColor CM64A (V2.05.13)", .files_no = 1,
                .internal_name = "NV5_VBIOS_POWERCOLOR_CM64A",
                .files = {NV5_VBIOS_POWERCOLOR_CM64A, ""} 
           },
           {
                .name = "[NV5] InnoVISION Inno3D TNT2 M64 (V2.05.13)", .files_no = 1,
                .internal_name = "NV5_VBIOS_INNOVISION_TNT2_M64",
                .files = {NV5_VBIOS_INNOVISION_TNT2_M64, ""},
           },
           {
                .name = "[NV5] Leadtek 16MB AGP (V3.05.00.10.56)", .files_no = 1,
                .internal_name = "NV5_VBIOS_LEADTEK_16MB",
                .files = {NV5_VBIOS_LEADTEK_16MB, ""},
           },
           {
                .name = "[NV5] Manli RIVA TNT2 M64 (V2.05.19.03.00)", .files_no = 1,
                .internal_name = "NV5_VBIOS_MANLI_RIVA_TNT2_M64",
                .files = {NV5_VBIOS_MANLI_RIVA_TNT2_M64, ""},
           },
           {
                .name = "[NV5] MSI MS-8808 (V2.05.20.02.25)", .files_no = 1,
                .internal_name = "NV5_VBIOS_MSI_MS_8808",
                .files = {NV5_VBIOS_MSI_MS_8808, ""},
           },
           {
                .name = "[NV5] Pine PV-502A-BR (V3.05.00.10.00)", .files_no = 1,
                .internal_name = "NV5_VBIOS_PINE_PV_502A_BR",
                .files = {NV5_VBIOS_PINE_PV_502A_BR, ""},
           },
           {
                .name = "[NV5] Sparkle RIVA TNT2 M64 (V3.05.00.10.00)", .files_no = 1,
                .internal_name = "NV5_VBIOS_SPARKLE",
                .files = {NV5_VBIOS_SPARKLE, ""},
           },
           {
                .name = "[NV5] Leadtek WinFast 3D 325 OEM (V4.8)", .files_no = 1,
                .internal_name = "NV5_VBIOS_LEADTEK_WINFAST_3D_325",
                .files = {NV5_VBIOS_LEADTEK_WINFAST_3D_325, ""},
           },
        }
    },
    // Memory configuration
    {
        .name = "VRAM",
        .description = "VRAM Size",
        .type = CONFIG_SELECTION,
        .default_int = 32,
        .selection = 
        {
            {
                .description = "16 MB",
                .value = 16,
            },
            {
                .description = "32 MB",
                .value = 32,
            },
        }

    },
    {
        .name = "Chip Revision",
        .description = "Chip Revision",
        .type = CONFIG_SELECTION,
        .default_int = NV5_PCI_CFG_REVISION_DEFAULT,
        .selection = 
        {
            {
               .description = "NV5 (Riva TNT2 Model 64) Revision ID Default",
               .value = NV5_PCI_CFG_REVISION_DEFAULT,
            },
        }
    },
    {
        .type = CONFIG_END
    }
};