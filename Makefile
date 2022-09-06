SHELL=/bin/bash

.PHONY: env
env:
	source ~/osbook/devenv/buildenv.sh
	echo $(CPPFLAGS)
	echo $(LDFLAGS)

.PHONY: build
build:
	clang++ $(CPPFLAGS) -O2 --target=x86_64-elf -fno-exceptions -ffreestanding -c ./kernel/main.cpp -o ./kernel/main.o
	ld.lld $(LDFLAGS) --entry KernelMain -z norelro --image-base 0x100000 --static -o ./kernel/kernel.elf ./kernel/main.o

.PHONY: qemu
qemu:
	source ~/edk2/edksetup.sh
	cd ~/edk2 && sh edkbuild.sh