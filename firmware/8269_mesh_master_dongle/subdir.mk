################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../div_mod.S 

OBJS += \
./div_mod.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 CC/Assembler'
	tc32-elf-gcc -Xassembler"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware" -DMCU_STARTUP_8267 -D__LOAD_RAM_SIZE__=0x12 -D__PROJECT_8267_MASTER_KMA_DONGLE__=1 -DCHIP_TYPE=CHIP_TYPE_8269 -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


