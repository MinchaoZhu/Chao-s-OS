DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL_LIB_OBJS=\
kernel/klib/list.o \
kernel/klib/map.o \

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
kernel/kernel.o \
kernel/interrupt.o \
kernel/timer.o \
kernel/ioport.o \
kernel/pmm.o \
kernel/vmm.o \
kernel/kmm.o \
kernel/task.o \
kernel/schedule.o \
kernel/vbe.o \
kernel/math.o \
kernel/psf.o \
kernel/terminal.o \
kernel/keyboard.o \
$(KERNEL_LIB_OBJS) \


OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(ARCHDIR)/font.o \
$(KERNEL_OBJS) \
$(FONT_OBJS)\
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(ARCHDIR)/font.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .S .psf

all: chaosos.kernel

chaosos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot chaosos.kernel

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.psf.o:
	objcopy -O elf32-i386 -B i386 -I binary $< $@

clean:
	rm -f chaosos.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: chaosos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp chaosos.kernel $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
