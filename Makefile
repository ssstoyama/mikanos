.PHONY: all
all: kernel boot

.PHONY: kernel src/kernel/kernel.elf
kernel: src/kernel/kernel.elf
src/kernel/kernel.elf:
	make -C src/kernel

.PHONY: boot
boot: Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi
Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi: src/MikanLoaderPkg/Main.c
	build

.PHONY: run
run: Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi src/kernel/kernel.elf
	# ${HOME}/osbook/devenv/run_qemu.sh $^
	APPS_DIR=apps RESOURCE_DIR=resource ./build.sh run

.PHONY: debug
debug: Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi src/kernel/kernel.elf
	${HOME}/osbook/devenv/run_dqemu.sh $^

.PHONY: clean
clean:
	make -C src/kernel clean

.PHONY: cleanall
cleanall:
	build cleanall
	make -C src/kernel clean

.PHONY: mount
mount:
	sudo mount -o loop disk.img mnt

.PHONY: umount
umount:
	sudo umount mnt
