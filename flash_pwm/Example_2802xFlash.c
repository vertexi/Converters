//###########################################################################
//
// FILE:    Example_2802xFlash.c
//
// TITLE:   f2802x EPwm Timer Interrupt From Flash Example.
//
// ASSUMPTIONS:
//
//    This program requires the f2802x header files.
//
//    As supplied, this project is configured for "boot to FLASH"
//    operation.  The 2802x Boot Mode table is shown below.
//    For information on configuring the boot mode of an eZdsp,
//    please refer to the documentation included with the eZdsp,
//
//    $Boot_Table
//    While an emulator is connected to your device, the TRSTn pin = 1,
//    which sets the device into EMU_BOOT boot mode. In this mode, the
//    peripheral boot modes are as follows:
//
//      Boot Mode:   EMU_KEY        EMU_BMODE
//                   (0xD00)         (0xD01)
//      ---------------------------------------
//      Wait         !=0x55AA        X
//      I/O          0x55AA          0x0000
//      SCI          0x55AA          0x0001
//      Wait         0x55AA          0x0002
//      Get_Mode     0x55AA          0x0003
//      SPI          0x55AA          0x0004
//      I2C          0x55AA          0x0005
//      OTP          0x55AA          0x0006
//      Wait         0x55AA          0x0007
//      Wait         0x55AA          0x0008
//      SARAM        0x55AA          0x000A
//      Flash        0x55AA          0x000B   <-- "Boot to Flash"
//      Wait         0x55AA          Other
//
//   Write EMU_KEY to 0xD00 and EMU_BMODE to 0xD01 via the debugger
//   according to the Boot Mode Table above. Build/Load project,
//   Reset the device, and Run example
//
//   $End_Boot_Table
//
//    The program must first be compiled and then programmed into the
//    flash.
//
//
// DESCRIPTION:
//
//    This example runs the EPwm interrupt example from flash.
//
//    1) Build the project
//    2) Flash the .out file into the device.
//    3) Set the hardware jumpers to boot to Flash
//    4) Use the included GEL file to load the project, symbols
//       defined within the project and the variables into the watch
//       window.
//
//    Steps that were taken to convert the EPwm example from RAM
//    to Flash execution:
//
//    - Change the linker cmd file to reflect the flash memory map.
//    - Make sure any initialized sections are mapped to Flash.
//      In SDFlash utility this can be checked by the View->Coff/Hex
//      status utility. Any section marked as "load" should be
//      allocated to Flash.
//    - Make sure there is a branch instruction from the entry to Flash
//      at 0x3F7FF6 to the beginning of code execution. This example
//      uses the DSP0x_CodeStartBranch.asm file to accomplish this.
//    - Set boot mode Jumpers to "boot to Flash"
//    - For best performance from the flash, modify the waitstates
//      and enable the flash pipeline as shown in this example.
//      Note: any code that manipulates the flash waitstate and pipeline
//      control must be run from RAM. Thus these functions are located
//      in their own memory section called ramfuncs.
//
//
//    EPwm1 Interrupt will run from RAM and puts the flash into sleep mode
//    EPwm2 Interrupt will run from RAM and puts the flash into standby mode
//    EPwm3 Interrupt will run from FLASH
//
//    As supplied:
//
//    All timers have the same period
//    The timers are started sync'ed
//    An interrupt is taken on a zero event for each EPwm timer
//
//       EPwm1: takes an interrupt every event
//       EPwm2: takes an interrupt every 2nd event
//       EPwm3: takes an interrupt every 3rd event
//
//    Thus the Interrupt count for EPwm1, EPwm4-EPwm6 should be equal
//    The interrupt count for EPwm2 should be about half that of EPwm1
//    and the interrupt count for EPwm3 should be about 1/3 that of EPwm1
//
//          Watch Variables:
//                 EPwm1TimerIntCount
//                 EPwm2TimerIntCount
//                 EPwm3TimerIntCount
//
//                 Toggle GPIO34 while in the background loop.
//
//###########################################################################
// $TI Release: F2802x Support Library v3.03.00.00 $
// $Release Date: Tue May 26 17:09:17 IST 2020 $
// $Copyright:
// Copyright (C) 2009-2020 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

//
// Function Prototypes
//
void InitEPwm1Example(void);
__interrupt void epwm1_isr(void);

//
// Globals
//
uint32_t  EPwm1TimerIntCount;
uint32_t  EPwm2TimerIntCount;
uint32_t  EPwm3TimerIntCount;
uint16_t  EPwm1_DB_Direction;
uint16_t  EPwm2_DB_Direction;
uint16_t  EPwm3_DB_Direction;

//
// Defines for the maximum Dead Band values
//
#define EPWM1_MAX_DB   0x03FF
#define EPWM2_MAX_DB   0x03FF
#define EPWM3_MAX_DB   0x03FF

#define EPWM1_MIN_DB   0x0100
#define EPWM2_MIN_DB   0
#define EPWM3_MIN_DB   0

//
// Defines that keep track of which way the Dead Band is moving
//
#define DB_UP   1
#define DB_DOWN 0

//
// These are defined by the linker
//
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;

//
// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped using
// the linker cmd file.
//
//#pragma CODE_SECTION(epwm1_isr, "ramfuncs");

//
// Main
//
void main(void)
{
  //
  // WARNING: Always ensure you call memcpy before running any functions from
  // RAM InitSysCtrl includes a call to a RAM based function and without a
  // call to memcpy first, the processor will go "into the weeds"
  //
#ifdef _FLASH
  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

  //
  // For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
  // These functions are in the f2802x_EPwm.c file
  //
  InitEPwm1Gpio();

  //
  // Step 3. Clear all interrupts and initialize PIE vector table:
  // Disable CPU interrupts
  //
  DINT;

  //
  // Initialize the PIE control registers to their default state.
  // The default state is all PIE interrupts disabled and flags
  // are cleared.
  // This function is found in the f2802x_PieCtrl.c file.
  //
  InitPieCtrl();

  //
  // Disable CPU interrupts and clear all CPU interrupt flags:
  //
  IER = 0x0000;
  IFR = 0x0000;

  //
  // Initialize the PIE vector table with pointers to the shell Interrupt
  // Service Routines (ISR).
  // This will populate the entire table, even if the interrupt
  // is not used in this example.  This is useful for debug purposes.
  // The shell ISR routines are found in f2802x_DefaultIsr.c.
  // This function is found in f2802x_PieVect.c.
  //
  InitPieVectTable();

  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

  //
  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  //
  EALLOW;            // This is needed to write to EALLOW protected registers
  PieVectTable.EPWM1_INT = &epwm1_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  //
  // Step 4. Initialize all the Device Peripherals:
  // Not required for this example
  //
  EALLOW;
  SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
  EDIS;

  EPwm1Regs.DBFED = 0x0200;
  InitEPwm1Example();

  EALLOW;
  SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
  EDIS;

  //
  // Step 5. User specific code, enable interrupts
  // Initialize counters
  //
  EPwm1TimerIntCount = 0;

  //
  // Enable CPU INT3 which is connected to EPWM1-3 INT
  //
  IER |= M_INT3;

  //
  // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
  //
  PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

  //
  // Enable global Interrupts and higher priority real-time debug events
  //
  EINT;               // Enable Global interrupt INTM
  ERTM;               // Enable Global realtime interrupt DBGM

  //
  // Step 6. IDLE loop. Just sit and loop forever (optional)
  //
  for(;;)
  {
    __asm("          NOP");
  }
}

//
// epwm1_isr -
//
__interrupt void
epwm1_isr(void)
{
  //    if(EPwm1_DB_Direction == DB_UP)
  //    {
  //        if(EPwm1Regs.DBFED < EPWM1_MAX_DB)
  //        {
  //            EPwm1Regs.DBFED++;
  //            EPwm1Regs.DBRED++;
  //        }
  //        else
  //        {
  //            EPwm1_DB_Direction = DB_DOWN;
  //            EPwm1Regs.DBFED--;
  //            EPwm1Regs.DBRED--;
  //        }
  //    }
  //    else
  //    {
  //        if(EPwm1Regs.DBFED == EPWM1_MIN_DB)
  //        {
  //            EPwm1_DB_Direction = DB_UP;
  //            EPwm1Regs.DBFED++;
  //            EPwm1Regs.DBRED++;
  //        }
  //        else
  //        {
  //            EPwm1Regs.DBFED--;
  //            EPwm1Regs.DBRED--;
  //        }
  //    }
  EPwm1TimerIntCount++;

  //
  // Clear INT flag for this timer
  //
  EPwm1Regs.ETCLR.bit.INT = 1;

  //
  // Acknowledge this interrupt to receive more interrupts from group 3
  //
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// InitEPwm1Example -
//
void
InitEPwm1Example()
{
  EPwm1Regs.TBPRD = 600;                        // Set timer period
  EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

  //
  // Setup TBCLK
  //
  EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;       // Clock ratio to SYSCLKOUT
  EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
  EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  //
  // Setup compare
  //
  EPwm1Regs.CMPA.half.CMPA = 300;

  //
  // Set actions
  //
  EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
  EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;          // Set PWM1A on Zero
  EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  //
  // Active Low PWMs - Setup Deadband
  //
  EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
  EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm1Regs.DBRED = EPWM1_MIN_DB;
  EPwm1Regs.DBFED = EPWM1_MIN_DB;
  EPwm1_DB_Direction = DB_UP;

  //
  // Interrupt where we will change the Deadband
  //
  EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event
}

//
// End of File
//

