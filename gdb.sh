#!/bin/sh
set -e
. ./iso.sh

gnome-terminal -- bash -c 'gdb -tui ./isodir/boot/chaosos.kernel'

qemu-system-$(./target-triplet-to-arch.sh $HOST) -s -S -m 256 -cdrom chaosos.iso
