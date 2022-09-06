SHELL=/bin/bash

.PHONY:
build:
	cd /workspaces/mikanos/kernel && make

.PHONY: qemu
qemu: build
	source ~/edk2/edksetup.sh
	cd ~/edk2 && sh edkbuild.sh