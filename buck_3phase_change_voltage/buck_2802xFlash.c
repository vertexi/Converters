// true 3-phase

// Included Files
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "math.h"

// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped using
// the linker cmd file.
#pragma CODE_SECTION(epwm1_isr, "ramfuncs");
#pragma CODE_SECTION(adc1_isr, "ramfuncs");
#pragma CODE_SECTION(adc_calculate, "ramfuncs");
#pragma CODE_SECTION(adc_error_clear, "ramfuncs");
#pragma CODE_SECTION(get_PI_signal1, "ramfuncs");
#pragma CODE_SECTION(get_adc_values, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc0, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc1, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc2, "ramfuncs");
#pragma DATA_SECTION(temp_a, "xfile");
#pragma DATA_SECTION(x, "xfile");
static int x[5];

#pragma DATA_SECTION(y, "xfile");
static int y = 0x4545;

#pragma DATA_SECTION(z, "xfile");
static long z = 0x12345678;

//#pragma CODE_SECTION(get_PI_signal0, "ramfuncs");
//#pragma CODE_SECTION(get_PI_signal1, "ramfuncs");
int16_t temp_a = 10;
// Function Prototypes
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);
__interrupt void xint1_isr(void);
__interrupt void xint2_isr(void);

void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);

// Globals
uint32_t EPwm1TimerIntCount;
uint16_t ConversionCount;

// Defines for the maximum Dead Band values
#define EPWM1_MAX_DB   0x0000
#define EPWM1_MIN_DB   0x0000
#define EPWM2_MIN_DB   0x0000

// These are defined by the linker
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;

// Function Prototypes
__interrupt void adc1_isr(void);
void Adc_Config(void);

void InitExternalInt(void);
void initPWM();
void InitGPIO(void);
void initTimer();
void initMyAdc();
void change_duty(void);
void get_adc_values(void);
void get_PI_signal1(float *error_list);

int32_t pre_storage_adc0(void);
int32_t pre_storage_adc1(void);
int32_t pre_storage_adc2(void);
void adc_error_clear(void);
void adc_calculate(void);

int INIT_DUTY1 = 10; // for pwm1
int INIT_DUTY2 = 10; // for pwm2
int INIT_DUTY3 = 10; // for pwm3
#define INIT_PI0 10; //
#define INIT_PI1 3; //
float error_list0[3] = {0,0,0};//{1,1,1};
float error_list1[3] = {0,0,0};//{0,0,3};

#define sample_size 32
int16_t ADC0[sample_size] = {0};
int16_t ADC1[sample_size] = {0};
int16_t ADC2[sample_size] = {0}; // The CCS compiler don't initialize array with 0

float ADC0_slope = 1.0413;
float ADC0_intercept = -0.0003;
float ADC1_slope = 1.0575;
float ADC1_intercept = 0.0208;
float ADC2_slope = 1.0575;
float ADC2_intercept = 0.0408;
float ADC0_ADJ = 0.0;
float ADC1_ADJ = 0.0;
float ADC2_ADJ = 0.0;

float slope0 = 5.5678;
float intercept0 = -1.2200;  //valtage1
float slope1 = 0.8967;
float intercept1 = -0.14471; //current1
float slope2 = 0.750788;
float intercept2 = -0.02822; //current2

volatile float adc_vol0 = 0;
volatile float adc_vol1 = 0;
volatile float adc_vol2 = 0;
float adc_value0 = 0; // voltage

#define TARGET_0_ADJ 0;
#define TARGET_k_ADJ 0;

#define EPWM1_PRD (3600)
#define ADC_PERIOD 200
float T_sam = 0.000200;
float P_arg0 = 0.05;
float I_arg0 = 0;
float P_arg1 = 0.8;//0.8
float I_arg1 = 0.1;//0.1

#define spwm_size 250
int16_t spwm_table[spwm_size] = {10   , 31   , 52   , 72   , 93   , 114  ,
                                  134  , 155  , 175  , 195  , 215  , 235  ,
                                  255  , 275  , 294  , 313  , 332  , 351  ,
                                  370  , 388  , 406  , 424  , 442  , 459  ,
                                  476  , 493  , 510  , 526  , 542  , 557  ,
                                  572  , 587  , 601  , 615  , 629  , 642  ,
                                  655  , 667  , 679  , 691  , 702  , 713  ,
                                  723  , 733  , 742  , 751  , 759  , 767  ,
                                  774  , 781  , 788  , 794  , 799  , 804  ,
                                  808  , 812  , 816  , 818  , 821  , 823  ,
                                  824  , 825  , 825  , 825  , 824  , 823  ,
                                  821  , 818  , 816  , 812  , 808  , 804  ,
                                  799  , 794  , 788  , 781  , 774  , 767  ,
                                  759  , 751  , 742  , 733  , 723  , 713  ,
                                  702  , 691  , 679  , 667  , 655  , 642  ,
                                  629  , 615  , 601  , 587  , 572  , 557  ,
                                  542  , 526  , 510  , 493  , 476  , 459  ,
                                  442  , 424  , 406  , 388  , 370  , 351  ,
                                  332  , 313  , 294  , 275  , 255  , 235  ,
                                  215  , 195  , 175  , 155  , 134  , 114  ,
                                  93   , 72   , 52   , 31   , 10   , -10  ,
                                  -31  , -52  , -72  , -93  , -114 , -134 ,
                                  -155 , -175 , -195 , -215 , -235 , -255 ,
                                  -275 , -294 , -313 , -332 , -351 , -370 ,
                                  -388 , -406 , -424 , -442 , -459 , -476 ,
                                  -493 , -510 , -526 , -542 , -557 , -572 ,
                                  -587 , -601 , -615 , -629 , -642 , -655 ,
                                  -667 , -679 , -691 , -702 , -713 , -723 ,
                                  -733 , -742 , -751 , -759 , -767 , -774 ,
                                  -781 , -788 , -794 , -799 , -804 , -808 ,
                                  -812 , -816 , -818 , -821 , -823 , -824 ,
                                  -825 , -825 , -825 , -824 , -823 , -821 ,
                                  -818 , -816 , -812 , -808 , -804 , -799 ,
                                  -794 , -788 , -781 , -774 , -767 , -759 ,
                                  -751 , -742 , -733 , -723 , -713 , -702 ,
                                  -691 , -679 , -667 , -655 , -642 , -629 ,
                                  -615 , -601 , -587 , -572 , -557 , -542 ,
                                  -526 , -510 , -493 , -476 , -459 , -442 ,
                                  -424 , -406 , -388 , -370 , -351 , -332 ,
                                  -313 , -294 , -275 , -255 , -235 , -215 ,
                                  -195 , -175 , -155 , -134 , -114 , -93  ,
                                  -72  , -52  , -31  , -10};

uint8_t adc_cal = 0;
uint8_t PID_cal = 0;

uint16_t signal_pwm_counter = 125;
uint32_t signal_begin_time = 0;

// Main
void main(void)
{
  // WARNING: Always ensure you call memcpy before running any functions from
  // RAM InitSysCtrl includes a call to a RAM based function and without a
  // call to memcpy first, the processor will go "into the weeds"
  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

  int i;
  for (i = 0; i < sample_size; i++)
  {
    ADC0[i] = 0;
    ADC1[i] = 0;
    ADC2[i] = 0;
  }

  x[0] = 2;
  x[1] = 3;
  x[2] = 4;
  error_list0[2] = INIT_PI0;

  // Step 1. Initialize System Control:
  // PLL, WatchDog, enable Peripheral Clocks
  // This example function is found in the f2802x_SysCtrl.c file.
  InitSysCtrl();

  // Step 2. Initialize GPIO:
  // This example function is found in the f2802x_Gpio.c file and
  // illustrates how to set the GPIO to it's default state.

  //InitGpio();  // Skipped for this example

  // Step 3. Clear all interrupts and initialize PIE vector table:
  // Disable CPU interrupts
  DINT;

  // Initialize the PIE control registers to their default state.
  // The default state is all PIE interrupts disabled and flags
  // are cleared.
  // This function is found in the f2802x_PieCtrl.c file.
  InitPieCtrl();

  // Disable CPU interrupts and clear all CPU interrupt flags
  IER = 0x0000;
  IFR = 0x0000;

  // Initialize the PIE vector table with pointers to the shell Interrupt
  // Service Routines (ISR).
  // This will populate the entire table, even if the interrupt
  // is not used in this example.  This is useful for debug purposes.
  // The shell ISR routines are found in f2802x_DefaultIsr.c.
  // This function is found in f2802x_PieVect.c.
  InitPieVectTable();

  InitEPwm1Gpio();
  //InitEPwm2Gpio();
  //InitEPwm3Gpio();
  InitGPIO();
  // InitExternalInt();


  initPWM();
  initTimer();
  initMyAdc();

  for(;;)
  {
//    __asm("          NOP");
    if (PID_cal == 1)
    {
      PID_cal = 0;
      get_PI_signal1(error_list1);
      change_duty();
    }
  }
}

void InitGPIO(void)
{
  EALLOW;
  GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;        // GPIO
  GpioDataRegs.GPADAT.bit.GPIO19 = 0;
  GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;         // output
  GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;
  EDIS;
}

void initPWM()
{
  /*--------------------- for epwm1 initialization ------------------------*/
  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  EALLOW;            // This is needed to write to EALLOW protected registers
  PieVectTable.EPWM1_INT = &epwm1_isr;
  //PieVectTable.EPWM2_INT = &epwm2_isr;
  //PieVectTable.EPWM3_INT = &epwm3_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  // Step 4. Initialize all the Device Peripherals:
  // Not required for this example
  EALLOW;
  SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
  EDIS;

  InitEPwm1Example();
  //InitEPwm2Example();
  //InitEPwm3Example();

  EALLOW;
  SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
  EDIS;

  // Step 5. User specific code, enable interrupts
  // Initialize counters
  EPwm1TimerIntCount = 0;

  // Enable CPU INT3 which is connected to EPWM1-3 INT
  IER |= M_INT3;

  // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
  PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
  //PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
  //PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
}

void initTimer()
{
  /*--------------------- for timers initialization ------------------------*/

  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  EALLOW;            // This is needed to write to EALLOW protected registers
  PieVectTable.TINT0 = &cpu_timer0_isr;
  PieVectTable.TINT1 = &cpu_timer1_isr;
  PieVectTable.TINT2 = &cpu_timer2_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  // Step 4. Initialize the Device Peripheral. This function can be
  //         found in f2802x_CpuTimers.c
  InitCpuTimers();        // For this example, only initialize the Cpu Timers

  // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
  // 60MHz CPU Freq, 1 second Period (in uSeconds)
  ConfigCpuTimer(&CpuTimer0, 60, 500000);
  ConfigCpuTimer(&CpuTimer1, 60, 80);
  ConfigCpuTimer(&CpuTimer2, 60, ADC_PERIOD);

  // To ensure precise timing, use write-only instructions to write to the
  // entire register. Therefore, if any of the configuration bits are changed
  // in ConfigCpuTimer and InitCpuTimers (in f2802x_CpuTimers.h), the
  // below settings must also be updated.
  CpuTimer0Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0
  CpuTimer1Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0
  CpuTimer2Regs.TCR.all = 0x4001; //write-only instruction to set TSS bit = 0

  // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
  // which is connected to CPU-Timer 1, and CPU int 14, which is connected
  // to CPU-Timer 2:
  IER |= M_INT1;
  IER |= M_INT13;
  IER |= M_INT14;

  // Enable TINT0 in the PIE: Group 1 interrupt 7
  PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

  // Enable global Interrupts and higher priority real-time debug events
  EINT;               // Enable Global interrupt INTM
  ERTM;               // Enable Global realtime interrupt DBGM
}

void initMyAdc()
{
  /*--------------------- for ADC initialization ------------------------*/

  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  EALLOW;             // This is needed to write to EALLOW protected register
  PieVectTable.ADCINT1 = &adc1_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  // Step 4. Initialize all the Device Peripherals
  InitAdc();  // For this example, init the ADC
  //AdcOffsetSelfCal();

  // 使能ADCINT1为INT1.1
  // Step 5. User specific code, enable interrupts:
  // Enable ADCINT1 in PIE
  PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Enable INT 1.1 in the PIE
  //     PieCtrlRegs.PIEIER1.bit.INTx2 = 1; // Enable INT 1.2 in the PIE
  IER |= M_INT1;                     // Enable CPU Interrupt 1
  EINT;                              // Enable Global interrupt INTM
  ERTM;                              // Enable Global realtime interrupt DBGM

  ConversionCount = 0;

  // Configure ADC
  // Note: Channel ADCINA4  will be double sampled to workaround the ADC 1st
  // sample issue for rev0 silicon errata
  EALLOW;

  // 设定ADC采样完成后触发中断，且需用户手动CLEAR FLAG后才能再次触发中断(non-continus mode)
  // ADCINT1 trips after AdcResults latch
  AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;

  AdcRegs.INTSEL1N2.bit.INT1E     = 1;    // Enabled ADCINT1
  AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;    // Disable ADCINT1 Continuous mode

  //     AdcRegs.INTSEL1N2.bit.INT2E     = 1;    // Enabled ADCINT2
  //     AdcRegs.INTSEL1N2.bit.INT2CONT  = 0;    // Disable ADCINT2 Continuous mode

  // 选择 EOC2 为 ADCINT1 触发，即 SOC2 对应的ADC采样完成后触发
  // setup EOC2 to trigger ADCINT1 to fire
  AdcRegs.INTSEL1N2.bit.INT1SEL   = 1;
  //    AdcRegs.INTSEL1N2.bit.INT2SEL   = 0; // EOC0 trigger ADCINT2

  // 设定 SOC 的采样源引脚
  // set SOC0 channel select to ADCINA6
  AdcRegs.ADCSOC0CTL.bit.CHSEL  = 1;
  AdcRegs.ADCSOC1CTL.bit.CHSEL  = 7; // DC voltage

  // 设置为 EOC 采样的触发条件，5为epwm1 soca, 1为 timer0
  // 现在我想要全手动软件控制，即为 0 ， software only.
  // set SOC0 start trigger on timer2, due to round-robin SOC0 converts first
  // then SOC1, SOC2, SOC3, SOC4
  AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 3;
  AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 3;

  // 设置采样时钟窗口
  // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  AdcRegs.ADCSOC0CTL.bit.ACQPS  = 11;
  AdcRegs.ADCSOC1CTL.bit.ACQPS  = 11;
  EDIS;
}

// epwm1_isr -
__interrupt void epwm1_isr(void)
{
  EPwm1TimerIntCount++;

  // Clear INT flag for this timer
  EPwm1Regs.ETCLR.bit.INT = 1;

  // Acknowledge this interrupt to receive more interrupts from group 3
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// InitEPwm1Example -
void InitEPwm1Example()
{
  EPwm1Regs.TBPRD = EPWM1_PRD;                   // Set timer period
  EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

  // Setup TBCLK
  EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
  EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  // Setup compare
  EPwm1Regs.CMPA.half.CMPA = (INIT_DUTY1*EPWM1_PRD)/100;

  // Set actions
  EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
  EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;          // Set PWM1A on Zero
  EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low PWMs - Setup Deadband
  EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
  EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm1Regs.DBRED = EPWM1_MIN_DB;
  EPwm1Regs.DBFED = EPWM1_MIN_DB;

  // Interrupt
  EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event
}

// InitEPwm2Example -
void InitEPwm2Example()
{
  EPwm2Regs.TBPRD = EPWM1_PRD;                        // Set timer period
  EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

  // Setup TBCLK
  EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;        // Disable phase loading
  EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  // Setup compare
  EPwm2Regs.CMPA.half.CMPA = INIT_DUTY2*EPWM1_PRD/100;

  // Set actions
  EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM2A on Zero
  EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm2Regs.AQCTLB.bit.CAU = AQ_SET;           // Set PWM2A on Zero
  EPwm2Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low complementary PWMs - setup the deadband
  EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
  EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm2Regs.DBRED = EPWM2_MIN_DB;
  EPwm2Regs.DBFED = EPWM2_MIN_DB;

  // Interrupt
  EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm2Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event
}

// InitEPwm1Example -
void InitEPwm3Example()
{
  EPwm3Regs.TBPRD = EPWM1_PRD;                   // Set timer period
  EPwm3Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm3Regs.TBCTR = 0x0000;                      // Clear counter

  // Setup TBCLK
  EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;        // Disable phase loading
  EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  // Setup compare
  EPwm3Regs.CMPA.half.CMPA = INIT_DUTY3*EPWM1_PRD/100;

  // Set actions
  EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
  EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm3Regs.AQCTLB.bit.CAU = AQ_SET;          // Set PWM1A on Zero
  EPwm3Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low PWMs - Setup Deadband
  EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
  EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm3Regs.DBRED = EPWM1_MIN_DB;
  EPwm3Regs.DBFED = EPWM1_MIN_DB;

  // Interrupt
  EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm3Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm3Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event
}

int32_t pre_storage_adc0(void)
{
  static int32_t adc_sum0 = 0;
  static uint8_t counter = 0;
  adc_sum0 -= ADC0[ConversionCount];
  ADC0[ConversionCount] = AdcResult.ADCRESULT1;
  adc_sum0 += AdcResult.ADCRESULT1;
  if (counter < sample_size)
  {
    counter++;
    return ((int32_t)(AdcResult.ADCRESULT1)*sample_size);
  }
  return adc_sum0;
}

int32_t pre_storage_adc1(void)
{
  static int32_t adc_sum1 = 0;
  static uint8_t counter = 0;
  adc_sum1 -= ADC1[ConversionCount];
  ADC1[ConversionCount] = AdcResult.ADCRESULT1;
  adc_sum1 += AdcResult.ADCRESULT1;
  if (counter < sample_size)
  {
    counter++;
    return ((int32_t)(AdcResult.ADCRESULT1)*sample_size);
  }
  return adc_sum1;
}

int32_t pre_storage_adc2(void)
{
  static int32_t adc_sum2 = 0;
  static uint8_t counter = 0;
  adc_sum2 -= ADC2[ConversionCount];
  ADC2[ConversionCount] = AdcResult.ADCRESULT3;
  adc_sum2 += AdcResult.ADCRESULT3;
  if (counter < sample_size)
  {
    counter++;
    return ((int32_t)(AdcResult.ADCRESULT3)*sample_size);
  }
  return adc_sum2;
}


int32_t adc_value_aver_0 = 0;
int32_t adc_value_aver_1 = 0;
int32_t adc_value_aver_2 = 0;
int32_t adc_value_aver_3 = 0;

__interrupt void adc1_isr(void)
{
  adc_value_aver_0 = pre_storage_adc0()/sample_size;  // DC voltage
//  adc_value_aver_1 = pre_storage_adc1()/sample_size;
//  adc_value_aver_2 = pre_storage_adc2()/sample_size;
//  adc_value_aver_0 = AdcResult.ADCRESULT2;
//  adc_value_aver_1 = AdcResult.ADCRESULT1;
//  adc_value_aver_2 = AdcResult.ADCRESULT3;
//  adc_value_aver_3 = AdcResult.ADCRESULT4;
  get_adc_values();
  (ConversionCount == sample_size-1) ? (ConversionCount = 0) : (ConversionCount++);

  PID_cal = 1;

  // Clear ADCINT1 flag reinitialize for next SOC
  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

  // Acknowledge interrupt to PIE
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}

float voltage_k = 0.3;
float virtual_signal = 0;

double adc_value1_buffer[200];
uint8_t adc_value1_counter = 0;

double adc_value0_buffer[200];
uint8_t adc_value0_counter = 0;

double adc_value2_buffer[200];
uint8_t adc_value2_counter = 0;

double adc_value3_buffer[200];
uint8_t adc_value3_counter = 0;

int16_t target0_buffer[200];
uint8_t target0_counter = 0;

int16_t differ_buffer[200];
uint8_t differ_counter = 0;

//int16_t toltal_buffer[400];
//int16_t toltal_counter = 0;

int16_t counter_buffer[200];
int16_t counter_counter = 0;

int32_t adc1_bias = 2000;
int32_t adc0_bias = 2000;

//int32_t current_adc0_bias = 1880;
//int32_t current_adc1_bias = 1880;
//int32_t current_adc2_bias = 1880;

int32_t current_adc0_bias = 1860;
int32_t current_adc1_bias = 1760;
int32_t current_adc2_bias = 1790;
void get_adc_values(void)
{
  adc_value0 = 0.02116*(adc_value_aver_0*3300>>12)+0.9837; // DC voltage
  adc_value0_buffer[adc_value0_counter] = adc_value_aver_0;
  (adc_value0_counter == 200-1) ? (adc_value0_counter = 0) : (adc_value0_counter++);
}

float P_error0 = 0;
float I_error0 = 0;
int PI0_HILIMIT = 90;
int PI0_LOLIMIT = 1;
double target_dc = 36;
float m=1;//kongzhicanshu
double PI1_decision = 10;

void change_duty(void)
{
  EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD*PI1_decision/100;
}

float P_error1 = 0;
float I_error1 = 0;
float PI1_HILIMIT = 99.0f;
float PI1_LOLIMIT = 3.0f;

float PI1_INT_HILIMIT = 1500.0f;
float PI1_INT_LOLIMIT = -1500.0f;

int PI1_I_en = 1;

void get_PI_signal1(float *error_list)
{
  // error_list[0]  diff error
  // error_list[1]  current error
  // error_list[2]  last PI signal
  static int first_flag = 0;

  if (first_flag < 50)
  {
    first_flag++;
  }

  error_list[0] = target_dc - adc_value0;
  error_list[1] += (PI1_I_en*error_list[0]);
  P_error1 = P_arg1*error_list[0];
  I_error1 = I_arg1*error_list[1];
  error_list[2] = P_error1 + I_error1;
  if(((error_list[2]>PI1_HILIMIT) && (I_error1 > 0)) || ((error_list[2]<PI1_LOLIMIT) && (I_error1 < 0)))
  {
      PI1_I_en = 0;
  } else {
      PI1_I_en = 1;
  }

  PI1_decision = error_list[2];

  if (error_list[1] > PI1_INT_HILIMIT)
  {
    error_list[1] = PI1_INT_HILIMIT;
  }
  else if (error_list[1] < PI1_INT_LOLIMIT)
  {
    error_list[1] = PI1_INT_LOLIMIT;
  }

  if (PI1_decision > PI1_HILIMIT)
  {
    PI1_decision = PI1_HILIMIT;
  }
  else if (PI1_decision < PI1_LOLIMIT)
  {
    PI1_decision = PI1_LOLIMIT;
  }

  return;
}

// cpu_timer0_isr -
__interrupt void cpu_timer0_isr(void)
{
  CpuTimer0.InterruptCount++;

  // Acknowledge this interrupt to receive more interrupts from group 1
  //Gpio_example1();
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// cpu_timer1_isr -
__interrupt void cpu_timer1_isr(void)
{
  CpuTimer1.InterruptCount++;

  // The CPU acknowledges the interrupt
  EDIS;
}

// cpu_timer2_isr -
__interrupt void cpu_timer2_isr(void)
{
  EALLOW;
  CpuTimer2.InterruptCount++;

  // The CPU acknowledges the interrupt.
  EDIS;
}

//int32_t low_value = 0;
//int32_t high_value = 0;
//int32_t low_value2 = 0;
//int32_t high_value2 = 0;
////
//// xint1_isr -
////
//int32_t temp_see = 1;
//int32_t temp_th_hi = 105;
//int32_t temp_th_lo = 25;
//
//int16_t phase_shift_corr = 235;
//
//__interrupt void xint1_isr(void)
//{
////  temp_see = CpuTimer1.InterruptCount - signal_begin_time;
////  if (temp_see > temp_th_hi || temp_see < temp_th_lo)
////  {
////    signal_pwm_counter = 125+phase_shift_corr;
////    signal_begin_time = CpuTimer1.InterruptCount;
////    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;   // GPIO34 is low
////
////    low_value = adc_value_aver_1;
////    low_value2 = adc_value_aver_0;
////    adc1_bias = (low_value+high_value)>>1;
////    adc0_bias = (low_value2+high_value2)>>1;
////  }
//
//  //
//  // Acknowledge this interrupt to get more from group 1
//  //
//  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//}
//
////
//// xint2_isr -
////
//__interrupt void xint2_isr(void)
//{
////  temp_see = CpuTimer1.InterruptCount - signal_begin_time;
////  if (temp_see > temp_th_hi || temp_see < temp_th_lo)
////  {
////    signal_pwm_counter = phase_shift_corr;
////    signal_begin_time = CpuTimer1.InterruptCount;
////    GpioDataRegs.GPBSET.bit.GPIO34 = 1;   // GPIO34 is high
////
////    high_value = adc_value_aver_1;
////    high_value2 = adc_value_aver_0;
////  }
//
//  //
//  // Acknowledge this interrupt to get more from group 1
//  //
//  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//}

//void InitExternalInt(void)
//{
//  //
//  // Interrupts that are used in this example are re-mapped to
//  // ISR functions found within this file.
//  //
//  EALLOW;            // This is needed to write to EALLOW protected registers
//  PieVectTable.XINT1 = &xint1_isr;
//  PieVectTable.XINT2 = &xint2_isr;
//  EDIS;      // This is needed to disable write to EALLOW protected registers
//
//  //
//  // Enable XINT1 and XINT2 in the PIE: Group 1 interrupt 4 & 5
//  // Enable INT1 which is connected to WAKEINT:
//  //
//  PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
//  PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Group 1 INT4
//  PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Group 1 INT5
//  IER |= M_INT1;                              // Enable CPU INT1
//  EINT;
//
//  //
//  // GPIO6 and GPIO7 are inputs
//  //
//  EALLOW;
//  GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;     // GPIO
//  GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;      // input
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 1;    // XINT1 Synch to SYSCLKOUT only
//
//  GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;     // GPIO
//  GpioCtrlRegs.GPADIR.bit.GPIO7 = 0;      // input
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 1;    // XINT2 Synch to SYSCLKOUT only
//
//  GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0x6F;
//  EDIS;
//
//  //
//  // GPIO6 is XINT1, GPIO7 is XINT2
//  //
//  EALLOW;
//  GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 6;   // XINT1 is GPIO0
//  GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 7;   // XINT2 is GPIO1
//  EDIS;
//
//  //
//  // Configure XINT1
//  //
//  XIntruptRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt
//  XIntruptRegs.XINT2CR.bit.POLARITY = 1;      // Rising edge interrupt
//
//  //
//  // Enable XINT1 and XINT2
//  //
//  XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1
//  XIntruptRegs.XINT2CR.bit.ENABLE = 1;        // Enable XINT2
//
//  //
//  // GPIO34 will go low inside each interrupt.  Monitor this on a scope
//  //
//  EALLOW;
//  GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;        // GPIO
//  GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;         // output
//  EDIS;
//}
