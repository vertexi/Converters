################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2802x_GlobalVariableDefs.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/source/F2802x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_codestartbranch.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_codestartbranch.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_defaultisr.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_defaultisr.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_piectrl.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_piectrl.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_pievect.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_pievect.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_sysctrl.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_sysctrl.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2802x_usdelay.obj: C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/source/f2802x_usdelay.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/headers/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x" --include_path="C:/Users/Apoptosis/Documents/GitHub/electronics_tryout/f2802x/common/include" --include_path="C:/Users/Apoptosis/Documents/GitHub/libraries/math/IQmath/c28/include" -g --define="_DEBUG" --define="LARGE_MODEL" --quiet --verbose_diagnostics --diag_warning=225 --diag_suppress=232 --diag_suppress=10063 --issue_remarks --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


