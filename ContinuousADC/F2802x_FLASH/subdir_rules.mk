################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --define=_DEBUG --define=LARGE_MODEL --define=FLASH -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2802x_CodeStartBranch.obj: C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --define=_DEBUG --define=LARGE_MODEL --define=FLASH -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2802x_GlobalVariableDefs.obj: C:/ti/controlSUITE/development_kits/~SupportFiles/source/DSP2802x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --define=_DEBUG --define=LARGE_MODEL --define=FLASH -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


