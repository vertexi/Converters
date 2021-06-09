################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Sensor_tx.cpp: ../Sensor_tx.ino
	@echo 'Building file: "$<"'
	@echo 'Invoking: Resource Custom Build Step'
	
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ./%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ProgramData/chocolatey/lib/energia/tools/energia-1.8.10E23/hardware/tools/msp430/bin/msp430-gcc-4.6.3.exe" -c -mmcu=msp430g2553 -DF_CPU=16000000L -DENERGIA_MSP_EXP430G2553LP -DENERGIA_ARCH_MSP430 -DENERGIA=23 -DARDUINO=10610 -I"C:/ProgramData/chocolatey/lib/energia/tools/energia-1.8.10E23/hardware/energia/msp430/cores/msp430" -I"C:/ProgramData/chocolatey/lib/energia/tools/energia-1.8.10E23/hardware/energia/msp430/variants/MSP-EXP430G2553LP" -I"C:/Users/Apoptosis/workspace_v10/Sensor_tx" -I"C:/Users/Apoptosis/Documents/Energia/libraries/msp430_cc1101_energia_v2" -I"C:/ProgramData/chocolatey/lib/energia/tools/energia-1.8.10E23/hardware/tools/msp430/msp430/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"   -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


