/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5: Defines core class names for debugging purposes
 *
 *
 *
 * Authors: aquaboxs, <aquaboxstudios87@gmail.com>
 *
 *          Copyright 2025 aquaboxs
 */
#include <stdio.h>
#include <stdint.h>
#include <86Box/86box.h>
#include <86Box/device.h>
#include <86Box/mem.h>
#include <86box/pci.h>
#include <86Box/rom.h> // DEPENDENT!!!
#include <86Box/video.h>
#include <86box/nv/vid_nv.h>
#include <86Box/nv/vid_nv5.h>

/* These are the object classes AS RECOGNISED BY THE GRAPHICS HARDWARE. */
/* The drivers implement a COMPLETELY DIFFERENT SET OF CLASSES. */

/* THERE CAN ONLY BE 32 CLASSES IN NV5 BECAUSE THE CLASS ID PART OF THE CONTEXT OF A GRAPHICS OBJECT IN PFIFO RAM HASH TABLE IS ONLY 5 BITS LONG! */

const char* nv5_class_names[] = 
{
    "NV5 INVALID class 0x00",
    "NV5 class 0x01: Beta factor",
    "NV5 class 0x02: ROP5 (32-bit) operation",
    "NV5 class 0x03: Chroma key",
    "NV5 class 0x04: Plane mask",
    "NV5 class 0x05: Clipping rectangle",
    "NV5 class 0x06: Pattern",
    "NV5 class 0x07: Rectangle",
    "NV5 class 0x08: Point",
    "NV5 class 0x09: Line",
    "NV5 class 0x0A: Lin (line without starting or ending pixel)",
    "NV5 class 0x0B: Triangle",
    "NV5 class 0x0C: Windows 95 GDI text acceleration",
    "NV5 class 0x0D: Memory to memory format",
    "NV5 class 0x0E: Scaled image from memory",
    "NV5 INVALID class 0x0F",
    "NV5 class 0x10: Blit",
    "NV5 class 0x11: Image",
    "NV5 class 0x12: Bitmap",
    "NV5 INVALID class 0x13",
    "NV5 class 0x14: Transfer to Memory",
    "NV5 class 0x15: Stretched image from CPU",
    "NV5 INVALID class 0x16",
    "NV5 class 0x17: Direct3D 5.0 accelerated textured triangle w/zeta buffer",
    "NV5 INVALID class 0x18",
    "NV5 INVALID class 0x19",
    "NV5 INVALID class 0x1A",
    "NV5 INVALID class 0x1B",
    "NV5 class 0x1C: Image in Memory",
    "NV5 INVALID class 0x1D",
    "NV5 INVALID class 0x1E",
    "NV5 INVALID class 0x1F",
    
};
