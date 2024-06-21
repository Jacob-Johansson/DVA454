################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/CCS/ARM_CM4F/%.obj: ../FreeRTOS/portable/CCS/ARM_CM4F/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/Lab6_Assignment_3" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="D:/MDH/4-Fourth year/Inbyggda I/My work/Lab5_teacher/Lab5_export/src" --include_path="C:/" --define=ccs="ccs" --define=DEBUG --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/portable/CCS/ARM_CM4F/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS/portable/CCS/ARM_CM4F/%.obj: ../FreeRTOS/portable/CCS/ARM_CM4F/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jjn20030/ti/Lab6_Assignment_3" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c129exl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="D:/MDH/4-Fourth year/Inbyggda I/My work/Lab5_teacher/Lab5_export/src" --include_path="C:/" --define=ccs="ccs" --define=DEBUG --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/portable/CCS/ARM_CM4F/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


