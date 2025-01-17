/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV5 PTIMER - PIT emulation
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


nv_register_t ptimer_registers[] = {
    { NV5_PTIMER_INTR, "PTIMER - Interrupt Status", NULL, NULL},
    { NV5_PTIMER_INTR_EN, "PTIMER - Interrupt Enable", NULL, NULL,},
    { NV5_PTIMER_NUMERATOR, "PTIMER - Numerator", NULL, NULL, },
    { NV5_PTIMER_DENOMINATOR, "PTIMER - Denominator", NULL, NULL, },
    { NV5_PTIMER_TIME_0_NSEC, "PTIMER - Time0", NULL, NULL, },
    { NV5_PTIMER_TIME_1_NSEC, "PTIMER - Time1", NULL, NULL, },
    { NV5_PTIMER_ALARM_NSEC, "PTIMER - Alarm", NULL, NULL, },
    { NV_REG_LIST_END, NULL, NULL, NULL}, // sentinel value 
};

// ptimer init code
void nv5_ptimer_init()
{
    nv_log("NV5: Initialising PTIMER...");

    nv_log("Done!\n");    
}

// Handles the PTIMER alarm interrupt
void nv5_ptimer_interrupt(uint32_t num)
{
    nv5->ptimer.interrupt_status |= (1 << num);

    nv5_pmc_handle_interrupts(true);
}

// Ticks the timer.
void nv5_ptimer_tick(double real_time)
{
    // do not divide by zero
    if (nv5->ptimer.clock_numerator == 0
    || nv5->ptimer.clock_denominator == 0)
        return; 

    // get the current time
    // Due to timer system limitations, the timer system is not capable of running at 100 megahertz. Therefore, we have to scale it down and then scale up the level of changes
    // to the state. 

    // See Envytools. We need to use the frequency as a source. 
    // But we need to figure out how many cycles actually occurred because this counts up every cycle...

    // Convert to microseconds
    double freq_base = (real_time / 1000000.0f) / ((double)1.0 / nv5->nvbase.memory_clock_frequency);

    double current_time = freq_base * ((double)nv5->ptimer.clock_numerator * NV5_86BOX_TIMER_SYSTEM_FIX_QUOTIENT) / (double)nv5->ptimer.clock_denominator; // *10.0?

    // Logging is suppressed when reading this register because it is read many times
    // So we only log when we update.

    // truncate it 
    nv5->ptimer.time += (uint64_t)current_time;


    nv_log("PTIMER time ticked (The value is now 0x%08x)\n", nv5->ptimer.time);

    // Check if the alarm has actually triggered...
    if (nv5->ptimer.time >= nv5->ptimer.alarm)
    {
        nv_log("PTIMER alarm interrupt fired because we reached TIME value 0x%08x\n", nv5->ptimer.alarm);
        nv5_ptimer_interrupt(NV5_PTIMER_INTR_ALARM);
    }
}

uint32_t nv5_ptimer_read(uint32_t address) 
{ 
    // always enabled

    nv_register_t* reg = nv_get_register(address, ptimer_registers, sizeof(ptimer_registers)/sizeof(ptimer_registers[0]));

    // Only log these when tehy actually tick
    if (address != NV5_PTIMER_TIME_0_NSEC
    && address != NV5_PTIMER_TIME_1_NSEC)
    {
        nv_log("NV5: PTIMER Read from 0x%08x", address);
    }
    

    uint32_t ret = 0x00;

    // if the register actually exists
    if (reg)
    {
        // on-read function
        if (reg->on_read)
            ret = reg->on_read();
        else
        {   
            // Interrupt state:
            // Bit 0: Alarm
            
            switch (reg->address)
            {
                case NV5_PTIMER_INTR:
                    ret = nv5->ptimer.interrupt_status;
                    break;
                case NV5_PTIMER_INTR_EN:
                    ret = nv5->ptimer.interrupt_enable;
                    break;
                case NV5_PTIMER_NUMERATOR:
                    ret = nv5->ptimer.clock_numerator; // 15:0
                    break;
                case NV5_PTIMER_DENOMINATOR:
                    ret = nv5->ptimer.clock_denominator ; //15:0
                    break;
                // 64-bit value
                // High part
                case NV5_PTIMER_TIME_0_NSEC:
                    ret = nv5->ptimer.time & 0xFFFFFFFF; //28:0
                    break;
                // Low part
                case NV5_PTIMER_TIME_1_NSEC:
                    ret = nv5->ptimer.time >> 32; // 31:5
                    break;
                case NV5_PTIMER_ALARM_NSEC: 
                    ret = nv5->ptimer.alarm; // 31:5
                    break;
            }

        }
        //TIME0 and TIME1 produce too much log spam that slows everything down 
        if (reg->address != NV5_PTIMER_TIME_0_NSEC
        && reg->address != NV5_PTIMER_TIME_1_NSEC)
        {
            if (reg->friendly_name)
                nv_log(": %s (value = 0x%08x)\n", reg->friendly_name, ret);
            else   
                nv_log("\n");
        }
    }
    else
    {
        nv_log(": Unknown register read (address=0x%08x), returning 0x00\n", address);
    }

    return ret;
}

void nv5_ptimer_write(uint32_t address, uint32_t value) 
{
    // before doing anything, check the subsystem enablement
    nv_register_t* reg = nv_get_register(address, ptimer_registers, sizeof(ptimer_registers)/sizeof(ptimer_registers[0]));

    nv_log("NV5: PTIMER Write 0x%08x -> 0x%08x", value, address);

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
                // Bit 0 - Alarm

                case NV5_PTIMER_INTR:
                    nv5->ptimer.interrupt_status &= ~value;
                    nv5_pmc_clear_interrupts();
                    break;

                // Interrupt enablement state
                case NV5_PTIMER_INTR_EN:
                    nv5->ptimer.interrupt_enable = value & 0x1;
                    break;
                // nUMERATOR
                case NV5_PTIMER_NUMERATOR:
                    nv5->ptimer.clock_numerator = value & 0xFFFF; // 15:0
                    break;
                case NV5_PTIMER_DENOMINATOR:
                    // prevent Div0
                    if (!value)
                        value = 1;

                    nv5->ptimer.clock_denominator = value & 0xFFFF; //15:0
                    break;
                // 64-bit value
                // High part
                case NV5_PTIMER_TIME_0_NSEC:
                    nv5->ptimer.time |= (value) & 0xFFFFFFE0; //28:0
                    break;
                // Low part
                case NV5_PTIMER_TIME_1_NSEC:
                    nv5->ptimer.time |= ((uint64_t)(value & 0xFFFFFFE0) << 32); // 31:5
                    break;
                case NV5_PTIMER_ALARM_NSEC: 
                    nv5->ptimer.alarm = value & 0xFFFFFFE0; // 31:5
                    break;
            }
        }
    }
}