# What is about migrate to CMake?
# Rules for directories are very annoying
# Non-standard requirements: xterm, kvm

linker_script := bootloader_mbr.ld
bootable_device_flag := "\x55\xaa"

mkdir := mkdir -p
rm := rm -f
dd := dd status=none
host_echo := $(shell which echo)

vm_start_timeout := 2

builddir := build
disk_image := $(builddir)/hdd.img
bootloader_builddir := $(builddir)/bootloader
dummyos_builddir := $(builddir)/dummyos

bootloader_objdir := $(bootloader_builddir)/obj
$(bootloader_objdir):
	@$(mkdir) $(bootloader_objdir)

bootloader_bindir := $(bootloader_builddir)/bin
$(bootloader_bindir):
	@$(mkdir) $(bootloader_bindir)

dummyos_objdir := $(dummyos_builddir)/obj
$(dummyos_objdir):
	@$(mkdir) $(dummyos_objdir)

dummyos_bindir := $(dummyos_builddir)/bin
$(dummyos_bindir):
	@$(mkdir) $(dummyos_bindir)

bootloader_objects := $(addprefix $(bootloader_objdir)/,bootloader_mbr.o)
dummyos_objs := $(addprefix $(dummyos_objdir)/,test_os_loader.o)

bootloader_binary := $(bootloader_bindir)/my_bootloader.bin
dummyos_bin := $(dummyos_bindir)/dummyos.bin

image_builddir := $(builddir)/image
mbr_code_image := $(image_builddir)/mbr_code.img
dummyos_image := $(image_builddir)/dummyos.img

$(bootloader_objdir)/%.o : %.s
	as -o $@ $<

$(bootloader_objects): | $(bootloader_objdir)



GDB_COMMAND = "gdb \
	-ex 'target remote localhost:1234' \
	-ex 'set architecture i8086' \
	-ex 'display /i ($$cs*16)+$$pc'\
	-ex 'hbr *0x7c00' \
	"

$(dummyos_objs): | $(dummyos_objdir)

$(dummyos_objdir)/%.o : %.s
	as -o $@ $<

$(bootloader_binary): $(bootloader_objects) | $(bootloader_bindir)
	ld -o $@ $< --script=$(linker_script)




$(dummyos_bin): $(dummyos_objs) | $(dummyos_bindir)
	ld -o $(dummyos_bin) $(dummyos_objs) --script=$(linker_script)



$(mbr_code_image): $(bootloader_binary) | $(image_builddir)
	objcopy -O binary -j boot_code $< $@


$(image_builddir):
	@$(mkdir) $(image_builddir)


$(dummyos_image): $(dummyos_bin) | $(image_builddir)
	objcopy -O binary -j boot_code $(dummyos_bin) $(dummyos_image)


partition_table := $(image_builddir)/partition_table.img
partition_table_generator_builddir := $(builddir)/$(partition_table_generator)
partition_table_generator := $(partition_table_generator_builddir)/generate_partition_table

$(partition_table_generator_builddir) :
	@$(mkdir) $(partition_table_generator_builddir)

$(partition_table_generator): | $(partition_table_generator_builddir)

	cd $(partition_table_generator_builddir) && cmake ../../tools/ && make


$(partition_table): | $(image_builddir) $(partition_table_generator)

	$(partition_table_generator) $(partition_table)

$(disk_image): $(mbr_code_image) $(dummyos_image) $(partition_table)

	@echo "---------------------------------------------------------------"
	@echo "- Creating disk image                                         -"
	@echo "---------------------------------------------------------------"

	@$(dd) if=/dev/zero of=$(disk_image) bs=512 count=4
	@echo "  Create empty disk ...................................... OK  "

	@$(dd) if=$(mbr_code_image) of=$(disk_image) conv=notrunc
	@echo "  Write bootloader code .................................. OK  "

	@$(dd) if=$(dummyos_image) of=$(disk_image) conv=notrunc \
		bs=512 seek=1
	@echo "  Copy OS bootstrap ...................................... OK  "


	@$(dd) if=$(partition_table) of=$(disk_image) conv=notrunc \
		bs=1 seek=446
	@echo "  Write partition table .................................. OK  "

# host_echo is a hack to print correct string.
# internal make echoing doesn't understand -e
	@$(host_echo) -en $(bootable_device_flag) | $(dd) of=$(disk_image) \
		seek=510 count=2 bs=1 conv=notrunc
	@echo "  Marked disk as bootable ................................ OK  "

	@$(host_echo) -en $(bootable_device_flag) | $(dd) of=$(disk_image) \
		seek=1022 count=2 bs=1 conv=notrunc
	@echo "  Marked first partition as bootable ..................... OK  "

all: $(disk_image)



clean:
	$(rm) -r $(builddir)



start: $(disk_image)
	@echo "Starting VM"
	kvm -hda $(disk_image)



start_debug: $(disk_image)

	@echo "Starting VM"
	kvm -s -S -hda $(disk_image) &
	@echo "Sleeping 5 seconds to give VM time to start"
	@sleep $(vm_start_timeout)
	@echo "Starting gdb"
	xterm -e "$(GDB_COMMAND)"



dump: $(disk_image)

	@hexdump -C $(disk_image)
