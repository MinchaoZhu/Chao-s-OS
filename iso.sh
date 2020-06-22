#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/chaosos.kernel isodir/boot/chaosos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "chaosos" {
	multiboot /boot/chaosos.kernel
}
EOF
grub-mkrescue -o chaosos.iso isodir
