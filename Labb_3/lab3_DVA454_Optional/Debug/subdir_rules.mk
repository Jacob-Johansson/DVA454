################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
interrupt.obj: C:/ti/TivaWare_C_Series-2.2.0.295/driverlib/interrupt.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/lab3_DVA454" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/driverlib" --include_path="C:/Users/jjn20030/ti/Files" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/lab3_DVA454" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/driverlib" --include_path="C:/Users/jjn20030/ti/Files" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

timer.obj: C:/ti/TivaWare_C_Series-2.2.0.295/driverlib/timer.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/lab3_DVA454" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/driverlib" --include_path="C:/Users/jjn20030/ti/Files" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uartstdio.obj: C:/ti/TivaWare_C_Series-2.2.0.295/utils/uartstdio.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/lab3_DVA454" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/driverlib" --include_path="C:/Users/jjn20030/ti/Files" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/inc" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


