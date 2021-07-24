// Included Files
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped using
// the linker cmd file.
#pragma CODE_SECTION(epwm1_isr, "ramfuncs");
#pragma CODE_SECTION(adc1_isr, "ramfuncs");
#pragma CODE_SECTION(get_PI_signal0, "ramfuncs");
#pragma CODE_SECTION(get_PI_signal1, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc0, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc1, "ramfuncs");
#pragma CODE_SECTION(pre_storage_adc2, "ramfuncs");
//#pragma CODE_SECTION(adc2_isr, "ramfuncs");

// Function Prototypes
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

void InitEPwm1Example(void);
void InitEPwm2Example(void);
__interrupt void epwm1_isr(void);

// Globals
uint32_t  EPwm1TimerIntCount;
uint16_t  EPwm1_DB_Direction;
uint16_t  EPwm2_DB_Direction;

// Defines for the maximum Dead Band values
#define EPWM1_MAX_DB   0x03FF
#define EPWM1_MIN_DB   0x0000
#define EPWM2_MIN_DB   0x0000
#define EPWM1_PRD 1200

// Defines that keep track of which way the Dead Band is moving
#define DB_UP   1
#define DB_DOWN 0

// These are defined by the linker
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;

// Function Prototypes
__interrupt void adc1_isr(void);
void Adc_Config(void);

// Globals
uint16_t LoopCount;
uint16_t ConversionCount;
#define sample_size 16
uint16_t ADC0[sample_size] = {0};
uint16_t ADC1[sample_size] = {0};
uint16_t ADC2[sample_size] = {0}; // The CCS compiler don't initialize array with 0

void initPWM();
void initTimer();
void initMyAdc();
void get_PI_signal0(float *error_list);
void get_PI_signal1(float *error_list);
int32_t pre_storage_adc0(void);
int32_t pre_storage_adc1(void);
int32_t pre_storage_adc2(void);

float INIT_DUTY0; // for pwm1
float INIT_DUTY1; // for pwm2 initialize in main()
#define INIT_PI0 20; // for pwm1+pwm2
#define INIT_PI1 1; // for pwm1/pwm2
float DUTY_HIGH = 80;
float error_list0[3] = {1,1,1};
float error_list1[3] = {0,0,0};

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

float adc_value0 = 0;// voltage1 47kohm/10kohm
float adc_value1 = 0;// current1
float adc_value2 = 0;// current2
float adc_value3 = 0;// current1/current2

float target_0 = 8; // expect value for valtage
float target_k = 1; // current 1/ current 2
#define TARGET_0_ADJ 0;
#define TARGET_k_ADJ 0;

#define ADC_PERIOD 100
float T_sam = 0.000100;
float P_arg0 = 50;
float I_arg0 = 1000;
float P_arg1 = 0.1;
float I_arg1 = 150;

#define spwm_size 512
float spwm_table[spwm_size] = {603.6815  , 611.0440  , 618.4048  , 625.7628  , 633.1169  , 640.4661  ,
                      647.8092  , 655.1450  , 662.4726  , 669.7907  , 677.0984  , 684.3944  ,
                      691.6777  , 698.9473  , 706.2019  , 713.4405  , 720.6620  , 727.8654  ,
                      735.0495  , 742.2133  , 749.3556  , 756.4755  , 763.5718  , 770.6435  ,
                      777.6894  , 784.7086  , 791.7000  , 798.6625  , 805.5951  , 812.4968  ,
                      819.3664  , 826.2030  , 833.0056  , 839.7730  , 846.5044  , 853.1986  ,
                      859.8547  , 866.4716  , 873.0484  , 879.5841  , 886.0777  , 892.5283  ,
                      898.9347  , 905.2962  , 911.6116  , 917.8802  , 924.1008  , 930.2727  ,
                      936.3948  , 942.4663  , 948.4862  , 954.4536  , 960.3676  , 966.2274  ,
                      972.0320  , 977.7806  , 983.4723  , 989.1062  , 994.6815  , 1000.1974 ,
                      1005.6531 , 1011.0476 , 1016.3803 , 1021.6502 , 1026.8566 , 1031.9988 ,
                      1037.0759 , 1042.0872 , 1047.0319 , 1051.9092 , 1056.7186 , 1061.4591 ,
                      1066.1302 , 1070.7310 , 1075.2610 , 1079.7194 , 1084.1055 , 1088.4187 ,
                      1092.6584 , 1096.8239 , 1100.9146 , 1104.9298 , 1108.8690 , 1112.7316 ,
                      1116.5169 , 1120.2245 , 1123.8537 , 1127.4040 , 1130.8749 , 1134.2659 ,
                      1137.5764 , 1140.8059 , 1143.9540 , 1147.0202 , 1150.0040 , 1152.9050 ,
                      1155.7227 , 1158.4567 , 1161.1066 , 1163.6720 , 1166.1525 , 1168.5478 ,
                      1170.8574 , 1173.0811 , 1175.2185 , 1177.2692 , 1179.2330 , 1181.1096 ,
                      1182.8987 , 1184.6000 , 1186.2132 , 1187.7382 , 1189.1746 , 1190.5223 ,
                      1191.7811 , 1192.9508 , 1194.0312 , 1195.0221 , 1195.9234 , 1196.7350 ,
                      1197.4567 , 1198.0884 , 1198.6301 , 1199.0816 , 1199.4429 , 1199.7139 ,
                      1199.8946 , 1199.9849 , 1199.9849 , 1199.8946 , 1199.7139 , 1199.4429 ,
                      1199.0816 , 1198.6301 , 1198.0884 , 1197.4567 , 1196.7350 , 1195.9234 ,
                      1195.0221 , 1194.0312 , 1192.9508 , 1191.7811 , 1190.5223 , 1189.1746 ,
                      1187.7382 , 1186.2132 , 1184.6000 , 1182.8987 , 1181.1096 , 1179.2330 ,
                      1177.2692 , 1175.2185 , 1173.0811 , 1170.8574 , 1168.5478 , 1166.1525 ,
                      1163.6720 , 1161.1066 , 1158.4567 , 1155.7227 , 1152.9050 , 1150.0040 ,
                      1147.0202 , 1143.9540 , 1140.8059 , 1137.5764 , 1134.2659 , 1130.8749 ,
                      1127.4040 , 1123.8537 , 1120.2245 , 1116.5169 , 1112.7316 , 1108.8690 ,
                      1104.9298 , 1100.9146 , 1096.8239 , 1092.6584 , 1088.4187 , 1084.1055 ,
                      1079.7194 , 1075.2610 , 1070.7310 , 1066.1302 , 1061.4591 , 1056.7186 ,
                      1051.9092 , 1047.0319 , 1042.0872 , 1037.0759 , 1031.9988 , 1026.8566 ,
                      1021.6502 , 1016.3803 , 1011.0476 , 1005.6531 , 1000.1974 , 994.6815  ,
                      989.1062  , 983.4723  , 977.7806  , 972.0320  , 966.2274  , 960.3676  ,
                      954.4536  , 948.4862  , 942.4663  , 936.3948  , 930.2727  , 924.1008  ,
                      917.8802  , 911.6116  , 905.2962  , 898.9347  , 892.5283  , 886.0777  ,
                      879.5841  , 873.0484  , 866.4716  , 859.8547  , 853.1986  , 846.5044  ,
                      839.7730  , 833.0056  , 826.2030  , 819.3664  , 812.4968  , 805.5951  ,
                      798.6625  , 791.7000  , 784.7086  , 777.6894  , 770.6435  , 763.5718  ,
                      756.4755  , 749.3556  , 742.2133  , 735.0495  , 727.8654  , 720.6620  ,
                      713.4405  , 706.2019  , 698.9473  , 691.6777  , 684.3944  , 677.0984  ,
                      669.7907  , 662.4726  , 655.1450  , 647.8092  , 640.4661  , 633.1169  ,
                      625.7628  , 618.4048  , 611.0440  , 603.6815  , 596.3185  , 588.9560  ,
                      581.5952  , 574.2372  , 566.8831  , 559.5339  , 552.1908  , 544.8550  ,
                      537.5274  , 530.2093  , 522.9016  , 515.6056  , 508.3223  , 501.0527  ,
                      493.7981  , 486.5595  , 479.3380  , 472.1346  , 464.9505  , 457.7867  ,
                      450.6444  , 443.5245  , 436.4282  , 429.3565  , 422.3106  , 415.2914  ,
                      408.3000  , 401.3375  , 394.4049  , 387.5032  , 380.6336  , 373.7970  ,
                      366.9944  , 360.2270  , 353.4956  , 346.8014  , 340.1453  , 333.5284  ,
                      326.9516  , 320.4159  , 313.9223  , 307.4717  , 301.0653  , 294.7038  ,
                      288.3884  , 282.1198  , 275.8992  , 269.7273  , 263.6052  , 257.5337  ,
                      251.5138  , 245.5464  , 239.6324  , 233.7726  , 227.9680  , 222.2194  ,
                      216.5277  , 210.8938  , 205.3185  , 199.8026  , 194.3469  , 188.9524  ,
                      183.6197  , 178.3498  , 173.1434  , 168.0012  , 162.9241  , 157.9128  ,
                      152.9681  , 148.0908  , 143.2814  , 138.5409  , 133.8698  , 129.2690  ,
                      124.7390  , 120.2806  , 115.8945  , 111.5813  , 107.3416  , 103.1761  ,
                      99.0854   , 95.0702   , 91.1310   , 87.2684   , 83.4831   , 79.7755   ,
                      76.1463   , 72.5960   , 69.1251   , 65.7341   , 62.4236   , 59.1941   ,
                      56.0460   , 52.9798   , 49.9960   , 47.0950   , 44.2773   , 41.5433   ,
                      38.8934   , 36.3280   , 33.8475   , 31.4522   , 29.1426   , 26.9189   ,
                      24.7815   , 22.7308   , 20.7670   , 18.8904   , 17.1013   , 15.4000   ,
                      13.7868   , 12.2618   , 10.8254   , 9.4777    , 8.2189    , 7.0492    ,
                      5.9688    , 4.9779    , 4.0766    , 3.2650    , 2.5433    , 1.9116    ,
                      1.3699    , 0.9184    , 0.5571    , 0.2861    , 0.1054    , 0.0151    ,
                      0.0151    , 0.1054    , 0.2861    , 0.5571    , 0.9184    , 1.3699    ,
                      1.9116    , 2.5433    , 3.2650    , 4.0766    , 4.9779    , 5.9688    ,
                      7.0492    , 8.2189    , 9.4777    , 10.8254   , 12.2618   , 13.7868   ,
                      15.4000   , 17.1013   , 18.8904   , 20.7670   , 22.7308   , 24.7815   ,
                      26.9189   , 29.1426   , 31.4522   , 33.8475   , 36.3280   , 38.8934   ,
                      41.5433   , 44.2773   , 47.0950   , 49.9960   , 52.9798   , 56.0460   ,
                      59.1941   , 62.4236   , 65.7341   , 69.1251   , 72.5960   , 76.1463   ,
                      79.7755   , 83.4831   , 87.2684   , 91.1310   , 95.0702   , 99.0854   ,
                      103.1761  , 107.3416  , 111.5813  , 115.8945  , 120.2806  , 124.7390  ,
                      129.2690  , 133.8698  , 138.5409  , 143.2814  , 148.0908  , 152.9681  ,
                      157.9128  , 162.9241  , 168.0012  , 173.1434  , 178.3498  , 183.6197  ,
                      188.9524  , 194.3469  , 199.8026  , 205.3185  , 210.8938  , 216.5277  ,
                      222.2194  , 227.9680  , 233.7726  , 239.6324  , 245.5464  , 251.5138  ,
                      257.5337  , 263.6052  , 269.7273  , 275.8992  , 282.1198  , 288.3884  ,
                      294.7038  , 301.0653  , 307.4717  , 313.9223  , 320.4159  , 326.9516  ,
                      333.5284  , 340.1453  , 346.8014  , 353.4956  , 360.2270  , 366.9944  ,
                      373.7970  , 380.6336  , 387.5032  , 394.4049  , 401.3375  , 408.3000  ,
                      415.2914  , 422.3106  , 429.3565  , 436.4282  , 443.5245  , 450.6444  ,
                      457.7867  , 464.9505  , 472.1346  , 479.3380  , 486.5595  , 493.7981  ,
                      501.0527  , 508.3223  , 515.6056  , 522.9016  , 530.2093  , 537.5274  ,
                      544.8550  , 552.1908  , 559.5339  , 566.8831  , 574.2372  , 581.5952  ,
                      588.9560  , 596.3185};
int spwm_counter = 0;
float spwm_coff = 1;


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
  INIT_DUTY0 = spwm_table[0];
  INIT_DUTY1 = spwm_table[0];
  spwm_counter = 0;
  target_0 += TARGET_0_ADJ;
  target_k += TARGET_k_ADJ;

  error_list0[2] = INIT_PI0;
  error_list1[2] = INIT_PI1;

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

  initPWM();

  initTimer();

  // initMyAdc();

  for(;;)
  {
    __asm("          NOP");
  }

}

void initPWM()
{
  /*--------------------- for epwm1 initialization ------------------------*/
  // Interrupts that are used in this example are re-mapped to
  // ISR functions found within this file.
  EALLOW;            // This is needed to write to EALLOW protected registers
  PieVectTable.EPWM1_INT = &epwm1_isr;
  EDIS;      // This is needed to disable write to EALLOW protected registers

  // Step 4. Initialize all the Device Peripherals:
  // Not required for this example
  EALLOW;
  SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
  EDIS;

  InitEPwm1Example();
  InitEPwm2Example();

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
  ConfigCpuTimer(&CpuTimer0, 60, ADC_PERIOD);
  ConfigCpuTimer(&CpuTimer1, 60, 40);
  ConfigCpuTimer(&CpuTimer2, 60, 10000);

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

  LoopCount = 0;
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
  AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 1;

  // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1
  AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 1;

  // set SOC2 start trigger on EPWM1A, due to round-robin SOC0 converts first
  // then SOC1, then SOC2
  AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 1;

  // 设置采样时钟窗口
  // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
  AdcRegs.ADCSOC0CTL.bit.ACQPS  = 6;
  AdcRegs.ADCSOC1CTL.bit.ACQPS  = 6;
  AdcRegs.ADCSOC2CTL.bit.ACQPS  = 6;
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
  EPwm1Regs.CMPA.half.CMPA = INIT_DUTY0;

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
  EPwm1_DB_Direction = DB_UP;

  // Interrupt where we will change the Deadband
  EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
  EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
  EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event
}

// InitEPwm2Example -
void InitEPwm2Example()
{
  EPwm2Regs.TBPRD = EPWM1_PRD;                        // Set timer period
  EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

  // Setup TBCLK
  EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
  EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  // Setup compare
  EPwm2Regs.CMPA.half.CMPA = INIT_DUTY1;

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
  EPwm2_DB_Direction = DB_UP;
}

int32_t pre_storage_adc0(void)
{
  static int32_t adc_sum0 = 0;
  adc_sum0 -= ADC0[ConversionCount];
  ADC0[ConversionCount] = AdcResult.ADCRESULT0;
  adc_sum0 += AdcResult.ADCRESULT0;
  return adc_sum0;
}


int32_t pre_storage_adc1(void)
{
  static int32_t adc_sum1 = 0;
  adc_sum1 -= ADC1[ConversionCount];
  ADC1[ConversionCount] = AdcResult.ADCRESULT1;
  adc_sum1 += AdcResult.ADCRESULT1;
  return adc_sum1;
}

int32_t pre_storage_adc2(void)
{
  static int32_t adc_sum2 = 0;
  adc_sum2 -= ADC2[ConversionCount];
  ADC2[ConversionCount] = AdcResult.ADCRESULT2;
  adc_sum2 += AdcResult.ADCRESULT2;
  return adc_sum2;
}

volatile float adc_vol0 = 0;
volatile float adc_vol1 = 0;
volatile float adc_vol2 = 0;

__interrupt void adc1_isr(void)
{
  volatile float temp;
  adc_vol0 = ((double)pre_storage_adc0()/(double)(sample_size*4096.0))*3.3*ADC0_slope+ADC0_intercept;
  temp = adc_vol0*slope0+intercept0;
  adc_value0 = temp;
  adc_vol1 = ((double)pre_storage_adc1()/(double)(sample_size*4096.0))*3.3*ADC1_slope+ADC1_intercept;
  temp = adc_vol1;
  adc_value1 = temp;
  adc_vol2 = ((double)pre_storage_adc2()/(double)(sample_size*4096.0))*3.3*ADC2_slope+ADC2_intercept;
  temp = adc_vol2;
  adc_value2 = temp;

  adc_value3 = adc_value1 / adc_value2;

  get_PI_signal0(error_list0);
  get_PI_signal1(error_list1);

  (ConversionCount == sample_size-1) ? (ConversionCount = 0) : (ConversionCount++);

  // Clear ADCINT1 flag reinitialize for next SOC
  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

  // Acknowledge interrupt to PIE
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  return;
}

float P_error0 = 0;
float I_error0 = 0;
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

  if (error_list[2] > DUTY_HIGH*2 && error_list[1] > 0)
  {
    I_en = 0;
  }else if (error_list[2] < 3 && error_list[1] < 0)
  {
    I_en = 0;
  }else
  {
    I_en = 1;
  }

  if (error_list[2] > DUTY_HIGH*2)
  {
    if (first_flag < 50)
    {
      error_list[2] = INIT_PI0;
      first_flag = 55;
    } else
    {
      error_list[2] = DUTY_HIGH*2;
    }
  } else if (error_list[2] < 3)
  {
    error_list[2] = 3;
  }

  return;
}

float P_error1 = 0;
float I_error1 = 0;
float pwm1_f = 0;
float pwm2_f = 0;
void get_PI_signal1(float *error_list)
{
  // error_list[1]  current error
  // error_list[0]  last error
  // error_list[2]  last PI signal
  static int I_en = 1;
  static int first_flag = 0;

  error_list[1] = target_k*0.76528+0.21723 - adc_value3;
  P_error1 = P_arg1*(error_list[1] - error_list[0]);
  I_error1 = I_en*I_arg1*(T_sam*error_list[1]);
  error_list[2] = error_list[2] + P_error1 + I_error1;

  error_list[0] = error_list[1];

  // allocation the pwm1 & pwm2 duty
  float pwm2 = 0, pwm1 = 0;
  pwm2 = error_list0[2]/(1+error_list1[2]);
  pwm1 = error_list1[2]*pwm2;

  // Integratation limitation of PI control algorithm
  if ((error_list[2] > 10||(pwm1 > DUTY_HIGH || pwm2 < 3)) && error_list[1] > 0)
  {
    I_en = 0;
  } else if ((error_list[2] < 0.1||(pwm2 > DUTY_HIGH || pwm1 < 3)) && error_list[1] < 0)
  {
    I_en = 0;
  } else
  {
    I_en = 1;
  }

  // keep track the first time final control signal limit in 0.1~10 range
  if (first_flag < 50)
  {
    first_flag++;
  }
  if (error_list[2] > 10 || error_list[2] < 0.1)
  {
    if (first_flag < 50)
    {
      error_list[2] = INIT_PI1;
      first_flag = 55;
    }
  }
  // keep track the first time pwm duty in 3~96 range
  if (pwm1 > DUTY_HIGH || pwm1 < 3)
  {
    if (first_flag < 50)
    {
      pwm1 = INIT_DUTY0;
      first_flag = 55;
    }
  }
  if (pwm2 > DUTY_HIGH || pwm2 < 3)
  {
    if (first_flag < 50)
    {
      pwm2 = INIT_DUTY1;
      first_flag = 55;
    }
  }

  // limitation the actual PI signal output
  if (error_list[2] > 10)
  {
    error_list[2] = 10;
  } else if (error_list[2] < 0.1)
  {
    error_list[2] = 0.1;
  }

  // limitation the proportion pwm duty
  if (pwm1 > DUTY_HIGH)
  {
    pwm1 = DUTY_HIGH;
  }
  if (pwm2 > DUTY_HIGH)
  {
    pwm2 = DUTY_HIGH;
  }
  if (pwm1 < 3)
  {
    pwm1 = 3;
  }
  if (pwm2 < 3)
  {
    pwm2 = 3;
  }

  pwm1_f = pwm1;
  pwm2_f = pwm2;
  EPwm1Regs.CMPA.half.CMPA = EPWM1_PRD-pwm1_f/100*EPWM1_PRD;
  EPwm2Regs.CMPA.half.CMPA = EPWM1_PRD-pwm2_f/100*EPWM1_PRD;
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

  (spwm_counter == spwm_size-1) ? (spwm_counter = 0) : (spwm_counter++);
  EPwm1Regs.CMPA.half.CMPA = spwm_coff*spwm_table[spwm_counter];
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
