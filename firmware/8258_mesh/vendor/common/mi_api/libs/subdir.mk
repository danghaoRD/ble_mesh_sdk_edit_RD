################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/common/mi_api/libs/mi_config.c 

OBJS += \
./vendor/common/mi_api/libs/mi_config.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/common/mi_api/libs/%.o: ../vendor/common/mi_api/libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware" -I"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware\vendor\common\mi_api\libs" -I"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware\vendor\common\mi_api\mijia_ble_api" -D__PROJECT_MESH__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


