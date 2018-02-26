################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
EK_TM4C123GXL.obj: ../EK_TM4C123GXL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="EK_TM4C123GXL.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

USBKBD.obj: ../USBKBD.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="USBKBD.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

usbkeyboarddevice.obj: ../usbkeyboarddevice.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="usbkeyboarddevice.d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-2055959681:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-2055959681-inproc

build-2055959681-inproc: ../usbkeyboarddevice.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/home/alex/ti/xdctools_3_32_02_25_core/xs" --xdcpath="/home/alex/ti/tirtos_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/home/alex/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;/home/alex/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C123GH6PM -r release -c "/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI\" --include_path=\"/home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI\" --include_path=\"/home/alex/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"/home/alex/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix\" --include_path=\"/home/alex/ti/ccsv7/tools/compiler/ti-cgt-arm_17.9.0.STS/include\" --define=ccs=\"ccs\" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-2055959681 ../usbkeyboarddevice.cfg
configPkg/compiler.opt: build-2055959681
configPkg/: build-2055959681


