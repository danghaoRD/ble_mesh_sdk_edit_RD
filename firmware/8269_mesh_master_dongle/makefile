################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include vendor/8267_master_kma_dongle/subdir.mk
-include proj/mcu_spec/subdir.mk
-include proj/mcu/subdir.mk
-include proj/drivers/subdir.mk
-include proj/common/subdir.mk
-include proj/app/subdir.mk
-include drivers/8258/subdir.mk
-include drivers/8258/flash/subdir.mk
-include boot/8258/subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LST += \
8267_master_kma_dongle.lst \

FLASH_IMAGE += \

SIZEDUMMY += \
sizedummy \


# All Target
all: pre-build main-build

# Main-build Target
main-build: ble_lt_mesh_edit_RD.elf secondary-outputs

# Tool invocations
ble_lt_mesh_edit_RD.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	tc32-elf-ld --gc-sections -L"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware\proj_lib" -T ../boot.link -o"ble_lt_mesh_edit_RD.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

8267_master_kma_dongle.lst: ble_lt_mesh_edit_RD.elf
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S ble_lt_mesh_edit_RD.elf  >"8267_master_kma_dongle.lst"
	@echo 'Finished building: $@'
	@echo ' '

: ble_lt_mesh_edit_RD.elf
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy ble_lt_mesh_edit_RD.elf
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ble_lt_mesh_edit_RD.elf
	@echo 'Invoking: Print Size'
	tc32-elf-size -t ble_lt_mesh_edit_RD.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(LST)$(SIZEDUMMY) ble_lt_mesh_edit_RD.elf
	-@echo ' '

pre-build:
	-"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware/getver.sh"
	-@echo ' '

post-build:
	-"Z:\RangDong\TaiLieu\Mesh\Code\sig_mesh_sdk_edit_RD\firmware/tl_check_fw.sh"   8269_mesh_master_dongle   ble_lt_mesh_edit_RD
	-@echo ' '

secondary-outputs: $(LST) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
