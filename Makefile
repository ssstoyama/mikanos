SHELL=/bin/bash

.PHONY: env
env:
	source ~/osbook/devenv/buildenv.sh

.PHONY: build
build: env
	cd /workspaces/mikanos/kernel && make

.PHONY: qemu
qemu: build
	source ~/edk2/edksetup.sh
	cd ~/edk2 && sh edkbuild.sh