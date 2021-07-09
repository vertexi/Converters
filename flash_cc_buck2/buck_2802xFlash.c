//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

//
// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped using
// the linker cmd file.
//
#pragma CODE_SECTION(epwm1_isr, "ramfuncs");
#pragma CODE_SECTION(adc1_isr, "ramfuncs");
#pragma CODE_SECTION(get_PI_signal1, "ramfuncs");
#pragma CODE_SECTION(get_PI_signal2, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc1, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc2, "ramfuncs");
//#pragma CODE_SECTION(adc2_isr, "ramfuncs");

//
// Function Prototypes
//
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

//
// Function Prototypes
//
void InitEPwm1Example(void);
void InitEPwm2Example(void);
__interrupt void epwm1_isr(void);

//
// Globals
//
uint32_t  EPwm1TimerIntCount;
uint16_t  EPwm1_DB_Direction;
uint16_t  EPwm2_DB_Direction;


//
// Defines for the maximum Dead Band values
//
#define EPWM1_MAX_DB   0x03FF
#define EPWM1_MIN_DB   0x0000
#define EPWM2_MIN_DB   0x0000
#define EPWM1_PRD 600



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
// Function Prototypes
//
__interrupt void adc1_isr(void);
__interrupt void adc2_isr(void);
void Adc_Config(void);

//
// Globals
//
uint16_t LoopCount;
uint16_t ConversionCount;
#define sample_size 16
uint16_t ADC1[sample_size] = {0};
uint16_t ADC2[sample_size] = {0}; // The CCS compiler don't initialize array with 0



void initPWM();
void initTimer();
void initMyAdc();
void get_PI_signal1(float *error_list);
void get_PI_signal2(float *error_list);
int32_t pre_storage_adc1(void);
int32_t pre_storage_adc2(void);

#define INIT_DUTY1 30;
#define INIT_DUTY2 50;
float error_list1[3] = {1,1,1};
float error_list2[3] = {1,1,1};

float slope1 = 0.8967;
float intercept1 = -0.14471; //current1
float slope2 = 0.750788;
float intercept2 = -0.02822; //current2
float ADC1_slope = 1.0575;
float ADC1_intercept = 0.0208;
float ADC2_slope = 1.0575;
float ADC2_intercept = 0.0408;
float ADC1_ADJ = 0;
float ADC2_ADJ = 0.0;
float adc_value1 = 0;
float adc_value2 = 0;

float target_1 = 0.1;
float target_2 = 0.4;
#define TARGET_1_ADJ 0;
#define TARGET_2_ADJ 0;

#define ADC_PERIOD 100
float T_sam = 0.000100;
float P_arg = 50;
float I_arg = 2000;
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
  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
  int i;
  for (i = 0; i < sample_size; i++)
  {
    ADC1[i] = 0;
    ADC2[i] = 0;
  }
  target_1 += TARGET_1_ADJ;
  target_2 += TARGET_2_ADJ;
  error_list1[2] = INIT_DUTY1;
  error_list2[2] = INIT_DUTY2;
  //
  // Step 1. Initialize System Control:
  // PLL, WatchDog, enable Peripheral Clocks
  // This example function is found in the f2802x_SysCtrl.c file.
  //
  InitSysCtrl();

  //
  // Step 2. Initialize GPIO:
  // This example function is found in the f2802x_Gpio.c file and
  // illustrates how to set the GPIO to it's default state.
  //
  //InitGpio();  // Skipped for this example

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
  // Disable CPU interrupts and clear all CPU interrupt flags
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

  InitEPwm1Gpio();
  InitEPwm2Gpio();

  initPWM();

  initTimer();

  initMyAdc();

  for(;;)
  {
    __asm("          NOP");
  }

}

void initPWM()
{
  /*--------------------- for epwm1 initialization ------------------------*/
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

  InitEPwm1Example();
  InitEPwm2Example();

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
}

void initTimer()
{
  /*--------------------- for timers initialization ------------------------*/

  //
  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  //
  EALLOW;            // This is needed to write to EALLOW protected registers
  PieVectTable.TINT0 = &cpu_timer0_isr;
  PieVectTable.TINT1 = &cpu_timer1_isr;
  PieVectTable.TINT2 = &cpu_timer2_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  //
  // Step 4. Initialize the Device Peripheral. This function can be
  //         found in f2802x_CpuTimers.c
  //
  InitCpuTimers();        // For this example, only initialize the Cpu Timers

  //
  // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
  // 60MHz CPU Freq, 1 second Period (in uSeconds)
  //
  ConfigCpuTimer(&CpuTimer0, 60, ADC_PERIOD);
  ConfigCpuTimer(&CpuTimer1, 60, 10000000);
  ConfigCpuTimer(&CpuTimer2, 60, 10000);

  //
  // To ensure precise timing, use write-only instructions to write to the
  // entire register. Therefore, if any of the configuration bits are changed
  // in ConfigCpuTimer and InitCpuTimers (in f2802x_CpuTimers.h), the
  // below settings must also be updated.
  //
  CpuTimer0Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0
  CpuTimer1Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0
  CpuTimer2Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0

  //
  // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
  // which is connected to CPU-Timer 1, and CPU int 14, which is connected
  // to CPU-Timer 2:
  //
  IER |= M_INT1;
  IER |= M_INT13;
  IER |= M_INT14;

  //
  // Enable TINT0 in the PIE: Group 1 interrupt 7
  //
  PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

  //
  // Enable global Interrupts and higher priority real-time debug events
  //
  EINT;               // Enable Global interrupt INTM
  ERTM;               // Enable Global realtime interrupt DBGM
}

void initMyAdc()
{
  /*--------------------- for ADC initialization ------------------------*/

  //
  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  //
  EALLOW;             // This is needed to write to EALLOW protected register
  PieVectTable.ADCINT1 = &adc1_isr;
  //    PieVectTable.ADCINT2 = &adc2_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  //
  // Step 4. Initialize all the Device Peripherals
  //
  InitAdc();  // For this example, init the ADC
  //AdcOffsetSelfCal();

  // 使能ADCINT1为INT1.1
  //
  // Step 5. User specific code, enable interrupts:
  // Enable ADCINT1 in PIE
  //
  PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Enable INT 1.1 in the PIE
  //     PieCtrlRegs.PIEIER1.bit.INTx2 = 1; // Enable INT 1.2 in the PIE
  IER |= M_INT1;                     // Enable CPU Interrupt 1
  EINT;                              // Enable Global interrupt INTM
  ERTM;                              // Enable Global realtime interrupt DBGM

  LoopCount = 0;
  ConversionCount = 0;

  //
  // Configure ADC
  // Note: Channel ADCINA4  will be double sampled to workaround the ADC 1st
  // sample issue for rev0 silicon errata
  //
  EALLOW;

  // 设定ADC采样完成后触发中断，且需用户手动CLEAR FLAG后才能再次触发中断(non-continus mode)
  //
  // ADCINT1 trips after AdcResults latch
  //
  AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;

  AdcRegs.INTSEL1N2.bit.INT1E     = 1;    // Enabled ADCINT1
  AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;    // Disable ADCINT1 Continuous mode

  //     AdcRegs.INTSEL1N2.bit.INT2E     = 1;    // Enabled ADCINT2
  //     AdcRegs.INTSEL1N2.bit.INT2CONT  = 0;    // Disable ADCINT2 Continuous mode

  // 选择 EOC2 为 ADCINT1 触发，即 SOC2 对应的ADC采样完成后触发
  //
  // setup EOC2 to trigger ADCINT1 to fire
  //
  AdcRegs.INTSEL1N2.bit.INT1SEL   = 2;
  //    AdcRegs.INTSEL1N2.bit.INT2SEL   = 0; // EOC0 trigger ADCINT2

  // 设定 SOC 的采样源引脚
  //
  // set SOC0 channel select to ADCINA4
  //
  //     AdcRegs.ADCSOC0CTL.bit.CHSEL  = 6;

  //
  // set SOC1 channel select to ADCINA4
  //
  AdcRegs.ADCSOC1CTL.bit.CHSEL  = 4;

  //
  // set SOC1 channel select to ADCINA2
  //
  AdcRegs.ADCSOC2CTL.bit.CHSEL  = 2;

  // 设置为 EOC 采样的触发条件，5为epwm1 soca
  // 现在我想要全手动软件控制，即为 0 ， software only.
  //
  // set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1
  //
  //      AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 2;

  //
  // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1
  //
  AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 1;

  //
  // set SOC2 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1, then SOC2
  //
  AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 1;

  // 设置采样时钟窗口
  //
  // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  //
  //      AdcRegs.ADCSOC0CTL.bit.ACQPS  = 6;

  //
  // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  //
  AdcRegs.ADCSOC1CTL.bit.ACQPS  = 6;

  //
  // set SOC2 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  //
  AdcRegs.ADCSOC2CTL.bit.ACQPS  = 6;
  EDIS;
}

//
// epwm1_isr -
//
__interrupt void epwm1_isr(void)
{
  EPwm1TimerIntCount++;

  // Clear INT flag for this timer
  EPwm1Regs.ETCLR.bit.INT = 1;

  // Acknowledge this interrupt to receive more interrupts from group 3
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// InitEPwm1Example -
//
void InitEPwm1Example()
{
  EPwm1Regs.TBPRD = EPWM1_PRD;                   // Set timer period
  EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

  //
  // Setup TBCLK
  //
  EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
  EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  //
  // Setup compare
  //
  EPwm1Regs.CMPA.half.CMPA = EPWM1_PRD-error_list1[2]/100*EPWM1_PRD;

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
// InitEPwm2Example -
//
void InitEPwm2Example()
{
  EPwm2Regs.TBPRD = EPWM1_PRD;                        // Set timer period
  EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

  //
  // Setup TBCLK
  //
  EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
  EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  //
  // Setup compare
  //
  EPwm2Regs.CMPA.half.CMPA = EPWM1_PRD-error_list2[2]/100*EPWM1_PRD;

  //
  // Set actions
  //
  EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM2A on Zero
  EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm2Regs.AQCTLB.bit.CAU = AQ_SET;           // Set PWM2A on Zero
  EPwm2Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  //
  // Active Low complementary PWMs - setup the deadband
  //
  EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
  EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm2Regs.DBRED = EPWM2_MIN_DB;
  EPwm2Regs.DBFED = EPWM2_MIN_DB;
  EPwm2_DB_Direction = DB_UP;
}


//
// adc_isr -
//
int32_t pre_storage_adc1(void)
{
  static int32_t adc_sum1 = 0;
  adc_sum1 -= ADC1[ConversionCount];
  ADC1[ConversionCount] = AdcResult.ADCRESULT1;
  adc_sum1 += AdcResult.ADCRESULT1;
  return adc_sum1;
}

//
// adc_isr -
//
int32_t pre_storage_adc2(void)
{
  static int32_t adc_sum2 = 0;
  adc_sum2 -= ADC2[ConversionCount];
  ADC2[ConversionCount] = AdcResult.ADCRESULT2;
  adc_sum2 += AdcResult.ADCRESULT2;
  return adc_sum2;
}

float adc_vol1 = 0;
float adc_vol2 = 0;
__interrupt void adc1_isr(void)
{
  adc_vol1 = ((double)pre_storage_adc1()/(double)(sample_size*4096.0))*3.3*ADC1_slope+ADC1_intercept;
  adc_value1 = adc_vol1*slope1+intercept1;
  adc_vol2 = ((double)pre_storage_adc2()/(double)(sample_size*4096.0))*3.3*ADC2_slope+ADC2_intercept;
  adc_value2 = adc_vol2*slope2+intercept2;

  get_PI_signal1(error_list1);
  get_PI_signal2(error_list2);

  (ConversionCount == sample_size-1) ? (ConversionCount = 0) : (ConversionCount++);

  // Clear ADCINT1 flag reinitialize for next SOC
  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

  // Acknowledge interrupt to PIE
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}

float adc0_result_int;
__interrupt void adc2_isr(void)
{
  adc0_result_int = (int)((AdcResult.ADCRESULT0/4096.0)*EPWM1_PRD);

  EPwm1Regs.CMPA.half.CMPA = adc0_result_int;

  //
  // Clear ADCINT1 flag reinitialize for next SOC
  //
  AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;

  //
  // Acknowledge interrupt to PIE
  //
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}
//
// cpu_timer0_isr -
//
__interrupt void cpu_timer0_isr(void)
{
  CpuTimer0.InterruptCount++;

  //
  // Acknowledge this interrupt to receive more interrupts from group 1
  //
  //Gpio_example1();
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


//
// cpu_timer1_isr -
//
__interrupt void cpu_timer1_isr(void)
{
  CpuTimer1.InterruptCount++;

  //
  // The CPU acknowledges the interrupt
  //
  EDIS;
}

//
// cpu_timer2_isr -
//
__interrupt void cpu_timer2_isr(void)
{
  EALLOW;
  CpuTimer2.InterruptCount++;

  //
  // The CPU acknowledges the interrupt.
  //
  EDIS;
}

float P_error1 = 0;
float I_error1 = 0;

void get_PI_signal1(float *error_list)
{
  // error_list[1]  current error
  // error_list[0]  last error
  // error_list[2]  last PI signal
  static int I_en = 1;
  static int first_flag = 0;

  if (first_flag < 50)
  {
    first_flag++;
  }

  error_list[1] = target_1 - adc_value1;
  P_error1 = P_arg*(error_list[1] - error_list[0]);
  I_error1 = I_en*I_arg*(T_sam*error_list[1]);
  error_list[2] = error_list[2] + P_error1 + I_error1;

  error_list[0] = error_list[1];

  if (error_list[2] > 96)
  {
    if (first_flag < 50)
    {
      error_list[2] = 1;
      first_flag = 55;
    } else
    {
      error_list[2] = 96;
    }
  } else if (error_list[2] < 0)
  {
    error_list[2] = 0;
  }

  EPwm1Regs.CMPA.half.CMPA = EPWM1_PRD-error_list[2]/100*EPWM1_PRD;

  if (error_list[2] > 96 && error_list[1] > 0)
  {
    I_en = 0;
    return;
  }
  if (error_list[2] < 0 && error_list[1] < 0)
  {
    I_en = 0;
    return;
  }

  I_en = 1;
  return;
}

float P_error2 = 0;
float I_error2 = 0;

void get_PI_signal2(float *error_list)
{
  // error_list[1]  current error
  // error_list[0]  last error
  // error_list[2]  last PI signal
  static int I_en = 1;
  static int first_flag = 0;


  if (first_flag < 50)
  {
    first_flag++;
  }

  error_list[1] = target_2 - adc_value2;
  P_error2 = P_arg*(error_list[1] - error_list[0]);
  I_error2 = I_en*I_arg*(T_sam*error_list[1]);
  error_list[2] = error_list[2] + P_error2 + I_error2;

  error_list[0] = error_list[1];

  if (error_list[2] > 96)
  {
    if (first_flag < 50)
    {
      error_list[2] = 1;
      first_flag = 55;
    } else
    {
      error_list[2] = 96;
    }
  } else if (error_list[2] < 0)
  {
    error_list[2] = 0;
  }

  EPwm2Regs.CMPA.half.CMPA = EPWM1_PRD-error_list[2]/100*EPWM1_PRD;

  if (error_list[2] > 96 && error_list[1] > 0)
  {
    I_en = 0;
    return;
  }
  if (error_list[2] < 0 && error_list[1] < 0)
  {
    I_en = 0;
    return;
  }

  I_en = 1;
  return;
}
