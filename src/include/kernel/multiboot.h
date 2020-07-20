#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

#define MULTIBOOT_FLAG_MEM 0x001
#define MULTIBOOT_FLAG_DEVICE 0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS 0x008
#define MULTIBOOT_FLAG_AOUT 0x010
#define MULTIBOOT_FLAG_ELF 0x020
#define MULTIBOOT_FLAG_MMAP 0x040
#define MULTIBOOT_FLAG_CONFIG 0x080
#define MULTIBOOT_FLAG_LOADER 0x100
#define MULTIBOOT_FLAG_APM 0x200
#define MULTIBOOT_FLAG_VBE 0x400

typedef struct multiboot
{
    size_t flags;
    size_t mem_lower;
    size_t mem_upper;
    size_t boot_device;
    size_t cmdline;
    size_t mods_count;
    size_t mods_addr;
    size_t num;
    size_t size;
    size_t addr;
    size_t shndx;
    size_t mmap_length;
    size_t mmap_addr;
    size_t drives_length;
    size_t drives_addr;
    size_t config_table;
    size_t boot_loader_name;
    size_t apm_table;
    size_t vbe_control_info;
    size_t vbe_mode_info;
    size_t vbe_mode;
    size_t vbe_interface_seg;
    size_t vbe_interface_off;
    size_t vbe_interface_len;
} multiboot;

#endif
