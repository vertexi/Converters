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
//#pragma CODE_SECTION(get_PI_signal0, "ramfuncs");
//#pragma CODE_SECTION(get_PI_signal1, "ramfuncs");

// Function Prototypes
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

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

void initPWM();
void initTimer();
void initMyAdc();
void get_PI_signal0(float *error_list);
//void get_PI_signal1(float *error_list);
int32_t pre_storage_adc0(void);
int32_t pre_storage_adc1(void);
int32_t pre_storage_adc2(void);
void adc_error_clear(void);
void adc_calculate(void);

float INIT_DUTY0; // for pwm1
float INIT_DUTY1; // for pwm2 initialize in main()
#define INIT_PI0 1; // for pwm1+pwm2
#define INIT_PI1 1; // for pwm1/pwm2
float error_list0[3] = {1,1,1};
float error_list1[3] = {0,0,0};

#define sample_size 70
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
float adc_value0 = 0;// voltage1 47kohm/10kohm
float adc_value1 = 0;// current1
float adc_value2 = 0;// current2
float adc_value3 = 0;// current1/current2

float target_0 = 8; // expect value for valtage
float target_k = 1; // current 1/ current 2
#define TARGET_0_ADJ 0;
#define TARGET_k_ADJ 0;

#define EPWM1_PRD 2400
#define ADC_PERIOD 500
float T_sam = 0.000500;
float P_arg0 = 50;
float I_arg0 = 1000;
float P_arg1 = 0.1;
float I_arg1 = 150;

#define spwm_size 250
uint16_t spwm_table[spwm_size] = {2160 , 2159 , 2158 , 2156 , 2153 , 2150 ,
                                  2146 , 2141 , 2135 , 2129 , 2123 , 2115 ,
                                  2107 , 2098 , 2089 , 2079 , 2068 , 2057 ,
                                  2045 , 2033 , 2020 , 2006 , 1992 , 1977 ,
                                  1962 , 1946 , 1929 , 1912 , 1895 , 1876 ,
                                  1858 , 1839 , 1819 , 1799 , 1779 , 1758 ,
                                  1737 , 1715 , 1693 , 1670 , 1647 , 1624 ,
                                  1600 , 1576 , 1552 , 1528 , 1503 , 1478 ,
                                  1452 , 1427 , 1401 , 1375 , 1349 , 1322 ,
                                  1296 , 1269 , 1242 , 1215 , 1188 , 1161 ,
                                  1134 , 1107 , 1080 , 1053 , 1026 , 999  ,
                                  972  , 945  , 918  , 891  , 864  , 838  ,
                                  811  , 785  , 759  , 733  , 708  , 682  ,
                                  657  , 632  , 608  , 584  , 560  , 536  ,
                                  513  , 490  , 467  , 445  , 423  , 402  ,
                                  381  , 361  , 341  , 321  , 302  , 284  ,
                                  265  , 248  , 231  , 214  , 198  , 183  ,
                                  168  , 154  , 140  , 127  , 115  , 103  ,
                                  92   , 81   , 71   , 62   , 53   , 45   ,
                                  37   , 31   , 25   , 19   , 14   , 10   ,
                                  7    , 4    , 2    , 1    , 0    , 0    ,
                                  1    , 2    , 4    , 7    , 10   , 14   ,
                                  19   , 25   , 31   , 37   , 45   , 53   ,
                                  62   , 71   , 81   , 92   , 103  , 115  ,
                                  127  , 140  , 154  , 168  , 183  , 198  ,
                                  214  , 231  , 248  , 265  , 284  , 302  ,
                                  321  , 341  , 361  , 381  , 402  , 423  ,
                                  445  , 467  , 490  , 513  , 536  , 560  ,
                                  584  , 608  , 632  , 657  , 682  , 708  ,
                                  733  , 759  , 785  , 811  , 838  , 864  ,
                                  891  , 918  , 945  , 972  , 999  , 1026 ,
                                  1053 , 1080 , 1107 , 1134 , 1161 , 1188 ,
                                  1215 , 1242 , 1269 , 1296 , 1322 , 1349 ,
                                  1375 , 1401 , 1427 , 1452 , 1478 , 1503 ,
                                  1528 , 1552 , 1576 , 1600 , 1624 , 1647 ,
                                  1670 , 1693 , 1715 , 1737 , 1758 , 1779 ,
                                  1799 , 1819 , 1839 , 1858 , 1876 , 1895 ,
                                  1912 , 1929 , 1946 , 1962 , 1977 , 1992 ,
                                  2006 , 2020 , 2033 , 2045 , 2057 , 2068 ,
                                  2079 , 2089 , 2098 , 2107 , 2115 , 2123 ,
                                  2129 , 2135 , 2141 , 2146 , 2150 , 2153 ,
                                  2156 , 2158 , 2159 , 2160};
volatile int spwm_counter1 = 0;  // init in main
volatile int spwm_counter2 = 0;  // init in main
volatile int spwm_counter3 = 0;  // init in main

int spwm0_0 = spwm_size/12;
int spwm1_0 = spwm_size/3+spwm_size/12;
int spwm2_0 = spwm_size*2/3+spwm_size/12;

float spwm_coff = 1;
float spwm_div = 1;
uint8_t adc_cal = 0;
uint8_t freq_chan = 0;
void freq_changer();
void pwm1_changer(void);
void pwm0_changer(void);
void pwm2_changer(void);

uint8_t want_freq = 50;
uint16_t PRD = 2400;
float spwm_scaler = 1;


int16_t adc_max = 0;
int16_t adc_min = 5000;
int16_t adc0_max = 0;
int16_t adc2_max = 0;

int16_t adc0_max_p = 0;
int16_t adc1_max_p = 0;
int16_t adc2_max_p = 0;

int16_t adc_max_index = 0;
int16_t adc_min_index = 0;

float adc_amplitude = 0;
float adc_cycle = 0;
float adc_phase = 0;

float phase1_amplitude = 0;
float phase1_freq = 50;
float phase1_phase = 0;

uint8_t phase_chan = 0;

uint16_t phase_change_counter = 0;
uint16_t phase_change_counter0 = 0;
uint16_t phase_change_counter2 = 0;

//uint16_t phase_change_period = 40;
int16_t adc_max_n1 = 4096;
int16_t adc_max_n2 = 4096;

int16_t phase_shift_cal = 0;

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
  spwm_counter1 = spwm0_0;
  spwm_counter2 = spwm1_0;
  spwm_counter3 = spwm2_0;
  target_0 += TARGET_0_ADJ;
  target_k += TARGET_k_ADJ;

  error_list0[2] = INIT_PI0;
  error_list1[2] = INIT_PI1;

  spwm_coff = INIT_PI0;

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
  InitEPwm2Gpio();
  InitEPwm3Gpio();

  initPWM();
  initTimer();
  initMyAdc();

  for(;;)
  {
    //    __asm("          NOP");
    if (phase_chan == 1)
    {
      pwm0_changer();
      pwm1_changer();
      pwm2_changer();
      phase_chan = 0;
    }
    if (adc_cal == 1)
    {
      adc_calculate();
      adc_cal = 0;
    }

    if (freq_chan == 1)
    {
      want_freq = phase1_freq;
      freq_changer();
      spwm_coff = spwm_div * spwm_scaler;

      freq_chan = 0;
    }
  }
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
  InitEPwm2Example();
  InitEPwm3Example();

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
  ConfigCpuTimer(&CpuTimer1, 60, 100);
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
  AdcRegs.INTSEL1N2.bit.INT1SEL   = 2;
  //    AdcRegs.INTSEL1N2.bit.INT2SEL   = 0; // EOC0 trigger ADCINT2

  // 设定 SOC 的采样源引脚
  // set SOC0 channel select to ADCINA6
  AdcRegs.ADCSOC0CTL.bit.CHSEL  = 6;
  AdcRegs.ADCSOC1CTL.bit.CHSEL  = 4;
  AdcRegs.ADCSOC2CTL.bit.CHSEL  = 2;

  // 设置为 EOC 采样的触发条件，5为epwm1 soca, 1为 timer0
  // 现在我想要全手动软件控制，即为 0 ， software only.
  // set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1
  AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 3;

  // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1
  AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 3;

  // set SOC2 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1, then SOC2
  AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 3;

  // 设置采样时钟窗口
  // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  AdcRegs.ADCSOC0CTL.bit.ACQPS  = 11;
  AdcRegs.ADCSOC1CTL.bit.ACQPS  = 11;
  AdcRegs.ADCSOC2CTL.bit.ACQPS  = 11;
  EDIS;
}

// epwm1_isr -
__interrupt void epwm1_isr(void)
{
  EPwm1TimerIntCount++;

  (spwm_counter1 == spwm_size-1) ? (spwm_counter1 = 0) : (spwm_counter1++);
  EPwm1Regs.CMPA.half.CMPA = spwm_coff*spwm_table[spwm_counter1];


  (spwm_counter2 == spwm_size-1) ? (spwm_counter2 = 0) : (spwm_counter2++);
  EPwm2Regs.CMPA.half.CMPA = spwm_coff*spwm_table[spwm_counter2];

  (spwm_counter3 == spwm_size-1) ? (spwm_counter3 = 0) : (spwm_counter3++);
  EPwm3Regs.CMPA.half.CMPA = spwm_coff*spwm_table[spwm_counter3];

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
  EPwm1Regs.CMPA.half.CMPA = spwm_table[spwm_counter1];

  // Set actions
  EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
  EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;          // Set PWM1A on Zero
  EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low PWMs - Setup Deadband
  EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
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
  EPwm2Regs.CMPA.half.CMPA = spwm_table[spwm_counter2];

  // Set actions
  EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM2A on Zero
  EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm2Regs.AQCTLB.bit.CAU = AQ_SET;           // Set PWM2A on Zero
  EPwm2Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low complementary PWMs - setup the deadband
  EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
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
  EPwm3Regs.CMPA.half.CMPA = spwm_table[spwm_counter3];

  // Set actions
  EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;            // Set PWM1A on Zero
  EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

  EPwm3Regs.AQCTLB.bit.CAU = AQ_SET;          // Set PWM1A on Zero
  EPwm3Regs.AQCTLB.bit.CAD = AQ_CLEAR;

  // Active Low PWMs - Setup Deadband
  EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
  EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
  EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
  EPwm3Regs.DBRED = EPWM1_MIN_DB;
  EPwm3Regs.DBFED = EPWM1_MIN_DB;

  // Interrupt
  EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm3Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm3Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event
}

void adc_error_clear(void)
{
  int i;
  for (i = 0; i < sample_size; i++)
  {
    ADC0[i] = 0;
    ADC1[i] = 0;
    ADC2[i] = 0;
  }

  adc0_max_p = adc0_max;
  adc1_max_p = adc_max;
  adc2_max_p = adc2_max;

  adc_max = 0;
  adc_min = 5000;
  adc0_max = 0;
  adc2_max = 0;

  adc_max_index = 0;
  adc_min_index = 0;
}

__interrupt void adc1_isr(void)
{
  ADC0[ConversionCount] = AdcResult.ADCRESULT0;
  ADC1[ConversionCount] = AdcResult.ADCRESULT1;
  ADC2[ConversionCount] = AdcResult.ADCRESULT2;
  (ConversionCount == sample_size-1) ? (adc_cal = 1,ConversionCount = 0) : (ConversionCount++);

  phase_chan = 1;

  // Clear ADCINT1 flag reinitialize for next SOC
  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

  // Acknowledge interrupt to PIE
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}

void pwm0_changer(void)
{
  int16_t adc_buffer = 0;
  adc_buffer = AdcResult.ADCRESULT0;
  if (adc0_max < adc_buffer)
  {
    adc0_max = adc_buffer;
  }

  if (phase_change_counter0-- > 0)
  {
    if ((adc0_max_p - 20) <= adc_buffer)
    {
      //phase_shift_cal = (want_freq-50)*2*spwm_size/360;
      spwm_counter1 = spwm0_0+phase_shift_cal;
      phase_change_counter0 = 1000000/ADC_PERIOD/want_freq;
    }
  }
}

void pwm1_changer(void)
{
  int16_t adc_buffer = 0;
  adc_buffer = AdcResult.ADCRESULT1;
  if (adc_max < adc_buffer)
  {
    adc_max = adc_buffer;
  }

  if (phase_change_counter-- > 0)
  {
    if (adc_max_n1 <= adc_buffer && adc_buffer >= adc_max_n2)
    {
      //        if (ADC1[ConversionCount-1]>ADC1[ConversionCount-2] &&
      //            ADC1[ConversionCount-2]>ADC1[ConversionCount-3]){}
      //phase_shift_cal = (want_freq-50)*2*spwm_size/360;
      spwm_counter2 = spwm0_0+phase_shift_cal;
      phase_change_counter = 1000000/ADC_PERIOD/want_freq;
    }
  }

  if (adc_min > AdcResult.ADCRESULT1)
  {
    adc_min = AdcResult.ADCRESULT1;
  }
}

void pwm2_changer(void)
{
  int16_t adc_buffer = 0;
  adc_buffer = AdcResult.ADCRESULT2;
  if (adc2_max < adc_buffer)
  {
    adc2_max = adc_buffer;
  }

  if (phase_change_counter2-- > 0)
  {
    if ((adc2_max_p - 20) <= adc_buffer)
    {
      //phase_shift_cal = (want_freq-50)*2*spwm_size/360;
      spwm_counter3 = spwm0_0+phase_shift_cal;
      phase_change_counter2 = 1000000/ADC_PERIOD/want_freq;
    }
  }
}

#define adc_cycle_buffer_size 24
int adc_cycle_counter = 0;
int adc_cycle_buffer[adc_cycle_buffer_size];

void adc_calculate(void)
{
  int i = sample_size-3;
  int max_search = 0;
  int min_search = 0;

  for ( ;i >= 2; i--)
  {
    if ((adc_max - 20) <= ADC1[i] &&
        ADC1[i] >= ADC1[i-1] && ADC1[i-1] >= ADC1[i-2] &&
        ADC1[i] >= ADC1[i+1] && ADC1[i+1] >= ADC1[i+2])
    {
      adc_max_index = i;
      adc_max_n1 = ADC1[i-2];
      adc_max_n2 = ADC1[i+2];
      max_search = 1;
      break;
    }
  }

  for ( ;i >= 2; i--)
  {
    if (ADC1[i] <= (adc_min+20) && ADC1[i] <= ADC1[i-1] && ADC1[i-1] <= ADC1[i-2] &&
        ADC1[i] <= ADC1[i+1] && ADC1[i+1] <= ADC1[i+2] && (adc_max_index - i)>10 && (adc_max_index - i)<30)
    {
      adc_min = ADC1[i];
      adc_min_index = i;
      min_search = 1;
      break;
    }
  }

  if (max_search == 1 && min_search == 1)
  {
    adc_cycle_buffer[adc_cycle_counter] = adc_max_index - adc_min_index;
    (adc_cycle_counter == adc_cycle_buffer_size-1) ? (adc_cycle_counter = 0) : (adc_cycle_counter++);
    if (adc_cycle_counter == 0)
    {
      adc_cycle = 0;
      int j;
      for (j = 0; j < adc_cycle_buffer_size; j++)
      {
        adc_cycle += adc_cycle_buffer[j];
      }
      adc_cycle = (adc_cycle/adc_cycle_buffer_size)*2;
      if (adc_cycle < 0) adc_cycle *= -1;
      phase1_freq = roundf(1/(T_sam*adc_cycle));
    }
  }

  adc_amplitude = (float)(adc_max - adc_min);
  phase1_amplitude = (adc_amplitude/4096.0)*3.3;

  adc_error_clear();
}

float P_error0 = 0;
float I_error0 = 0;
float PI0_HILIMIT = 1;
float PI0_LOLIMIT = 0.1;
void get_PI_signal0(float *error_list)
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

  error_list[1] = target_0 - adc_value0;
  P_error0 = P_arg0*(error_list[1] - error_list[0]);
  I_error0 = I_en*I_arg0*(T_sam*error_list[1]);
  error_list[2] = error_list[2] + P_error0 + I_error0;

  error_list[0] = error_list[1];

  if (error_list[2] > PI0_HILIMIT && error_list[1] > 0)
  {
    I_en = 0;
  }else if (error_list[2] < PI0_LOLIMIT && error_list[1] < 0)
  {
    I_en = 0;
  }else
  {
    I_en = 1;
  }

  if (error_list[2] > PI0_HILIMIT)
  {
    if (first_flag < 50)
    {
      error_list[2] = INIT_PI0;
      first_flag = 55;
    } else
    {
      error_list[2] = PI0_HILIMIT;
    }
  } else if (error_list[2] < PI0_LOLIMIT)
  {
    error_list[2] = PI0_LOLIMIT;
  }

  return;
}

// cpu_timer0_isr -
__interrupt void cpu_timer0_isr(void)
{
  CpuTimer0.InterruptCount++;
  freq_chan = 1;

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

void freq_changer()
{
  //(want_freq == 60) ? (want_freq = 40) : (want_freq++);
  PRD = 60000000/(spwm_size*2)/want_freq;
  EPwm1Regs.TBPRD = PRD;
  EPwm2Regs.TBPRD = PRD;
  EPwm3Regs.TBPRD = PRD;

  spwm_scaler = PRD/2400.0;
}
