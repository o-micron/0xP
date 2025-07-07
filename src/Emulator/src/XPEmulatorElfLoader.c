#include <Emulator/XPEmulatorElfLoader.h>

#if defined(_WIN32) || defined(_WIN64)
    #pragma warning(disable : 4996)
    #define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// ELF constants
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_OSABI   7
#define EI_PAD     8
#define EI_NIDENT  16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ET_EXEC 2
#define ET_DYN  3

#define EM_RISCV 243

#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

// Program header flags
#define PF_X 1
#define PF_W 2
#define PF_R 4

// ELF header structures
typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint32_t      e_entry;
    uint32_t      e_phoff;
    uint32_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf32_Ehdr;

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf64_Ehdr;

// Program header structures
typedef struct
{
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

typedef struct
{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

// Byte swapping functions
static uint16_t
swap16(uint16_t value)
{
    return ((value & 0xff) << 8) | ((value >> 8) & 0xff);
}

static uint32_t
swap32(uint32_t value)
{
    return ((value & 0xff) << 24) | (((value >> 8) & 0xff) << 16) | (((value >> 16) & 0xff) << 8) |
           ((value >> 24) & 0xff);
}

static uint64_t
swap64(uint64_t value)
{
    return ((value & 0xffULL) << 56) | (((value >> 8) & 0xffULL) << 48) | (((value >> 16) & 0xffULL) << 40) |
           (((value >> 24) & 0xffULL) << 32) | (((value >> 32) & 0xffULL) << 24) | (((value >> 40) & 0xffULL) << 16) |
           (((value >> 48) & 0xffULL) << 8) | ((value >> 56) & 0xffULL);
}

// Endianness correction functions
static uint16_t
fix_endian16(RiscvElfLoader* loader, uint16_t value)
{
    return loader->is_little_endian ? value : swap16(value);
}

static uint32_t
fix_endian32(RiscvElfLoader* loader, uint32_t value)
{
    return loader->is_little_endian ? value : swap32(value);
}

static uint64_t
fix_endian64(RiscvElfLoader* loader, uint64_t value)
{
    return loader->is_little_endian ? value : swap64(value);
}

// Initialize the ELF loader
static RiscvElfLoader*
elf_loader_create(const char* filename)
{
    RiscvElfLoader* loader = malloc(sizeof(RiscvElfLoader));
    if (!loader) { return NULL; }

    memset(loader, 0, sizeof(RiscvElfLoader));

    loader->filename = malloc(strlen(filename) + 1);
    if (!loader->filename) {
        free(loader);
        return NULL;
    }
    strcpy(loader->filename, filename);

    loader->is_little_endian = true;

    return loader;
}

// Free the ELF loader
static void
elf_loader_destroy(RiscvElfLoader* loader)
{
    if (!loader) return;

    // Free segments
    for (int i = 0; i < loader->num_segments; i++) {
        if (loader->segments[i].data) { free(loader->segments[i].data); }
    }

    if (loader->file_data) { free(loader->file_data); }

    if (loader->filename) { free(loader->filename); }

    free(loader);
}

// Load file into memory
static bool
load_file(RiscvElfLoader* loader)
{
    FILE* file = fopen(loader->filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", loader->filename);
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    loader->file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory
    loader->file_data = malloc(loader->file_size);
    if (!loader->file_data) {
        fprintf(stderr, "Failed to allocate memory for file\n");
        fclose(file);
        return false;
    }

    // Read file
    size_t bytes_read = fread(loader->file_data, 1, loader->file_size, file);
    fclose(file);

    if (bytes_read != loader->file_size) {
        fprintf(stderr, "Failed to read complete file\n");
        return false;
    }

    return true;
}

// Validate ELF header
static bool
validate_elf_header(RiscvElfLoader* loader)
{
    if (loader->file_size < EI_NIDENT) {
        fprintf(stderr, "File too small to be valid ELF\n");
        return false;
    }

    uint8_t* ident = loader->file_data;

    // Check ELF magic
    if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file\n");
        return false;
    }

    // Check class
    if (ident[EI_CLASS] != ELFCLASS32 && ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "Unsupported ELF class\n");
        return false;
    }

    // Check data encoding
    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB) {
        fprintf(stderr, "Unsupported ELF data encoding\n");
        return false;
    }

    loader->is_64bit         = (ident[EI_CLASS] == ELFCLASS64);
    loader->is_little_endian = (ident[EI_DATA] == ELFDATA2LSB);

    return true;
}

// Parse 32-bit program headers
static bool
parse_program_headers_32(RiscvElfLoader* loader, Elf32_Ehdr* header)
{
    uint32_t phoff     = fix_endian32(loader, header->e_phoff);
    uint16_t phnum     = fix_endian16(loader, header->e_phnum);
    uint16_t phentsize = fix_endian16(loader, header->e_phentsize);

    if (phoff + phnum * phentsize > loader->file_size) {
        fprintf(stderr, "Program headers extend beyond file size\n");
        return false;
    }

    for (int i = 0; i < phnum && loader->num_segments < MAX_SEGMENTS; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(loader->file_data + phoff + i * phentsize);

        uint32_t type = fix_endian32(loader, phdr->p_type);
        if (type == PT_LOAD) {
            uint32_t vaddr  = fix_endian32(loader, phdr->p_vaddr);
            uint32_t filesz = fix_endian32(loader, phdr->p_filesz);
            uint32_t memsz  = fix_endian32(loader, phdr->p_memsz);
            uint32_t offset = fix_endian32(loader, phdr->p_offset);
            uint32_t flags  = fix_endian32(loader, phdr->p_flags);

            if (offset + filesz > loader->file_size) {
                fprintf(stderr, "Segment extends beyond file size\n");
                return false;
            }

            MemorySegment* segment = &loader->segments[loader->num_segments];
            segment->vaddr         = vaddr;
            segment->size          = memsz;
            segment->flags         = flags;
            segment->data          = malloc(memsz);

            if (!segment->data) {
                fprintf(stderr, "Failed to allocate segment memory\n");
                return false;
            }

            // Initialize with zeros
            memset(segment->data, 0, memsz);

            // Copy file data
            if (filesz > 0) { memcpy(segment->data, loader->file_data + offset, filesz); }

            loader->num_segments++;

            printf("Loaded segment: VA=0x%x, size=0x%x, flags=0x%x\n", vaddr, memsz, flags);
        }
    }

    return true;
}

// Parse 64-bit program headers
static bool
parse_program_headers_64(RiscvElfLoader* loader, Elf64_Ehdr* header)
{
    uint64_t phoff     = fix_endian64(loader, header->e_phoff);
    uint16_t phnum     = fix_endian16(loader, header->e_phnum);
    uint16_t phentsize = fix_endian16(loader, header->e_phentsize);

    if (phoff + phnum * phentsize > loader->file_size) {
        fprintf(stderr, "Program headers extend beyond file size\n");
        return false;
    }

    for (int i = 0; i < phnum && loader->num_segments < MAX_SEGMENTS; i++) {
        Elf64_Phdr* phdr = (Elf64_Phdr*)(loader->file_data + phoff + i * phentsize);

        uint32_t type = fix_endian32(loader, phdr->p_type);
        if (type == PT_LOAD) {
            uint64_t vaddr  = fix_endian64(loader, phdr->p_vaddr);
            uint64_t filesz = fix_endian64(loader, phdr->p_filesz);
            uint64_t memsz  = fix_endian64(loader, phdr->p_memsz);
            uint64_t offset = fix_endian64(loader, phdr->p_offset);
            uint32_t flags  = fix_endian32(loader, phdr->p_flags);

            if (offset + filesz > loader->file_size) {
                fprintf(stderr, "Segment extends beyond file size\n");
                return false;
            }

            MemorySegment* segment = &loader->segments[loader->num_segments];
            segment->vaddr         = vaddr;
            segment->size          = memsz;
            segment->flags         = flags;
            segment->data          = malloc(memsz);

            if (!segment->data) {
                fprintf(stderr, "Failed to allocate segment memory\n");
                return false;
            }

            // Initialize with zeros
            memset(segment->data, 0, memsz);

            // Copy file data
            if (filesz > 0) { memcpy(segment->data, loader->file_data + offset, filesz); }

            loader->num_segments++;

            printf("Loaded segment: VA=0x%llx, size=0x%llx, flags=0x%x\n", vaddr, memsz, flags);
        }
    }

    return true;
}

// Parse ELF header
static bool
parse_elf_header(RiscvElfLoader* loader)
{
    if (loader->is_64bit) {
        if (loader->file_size < sizeof(Elf64_Ehdr)) {
            fprintf(stderr, "File too small for 64-bit ELF header\n");
            return false;
        }

        Elf64_Ehdr* header = (Elf64_Ehdr*)loader->file_data;

        if (fix_endian16(loader, header->e_machine) != EM_RISCV) {
            fprintf(stderr, "Not a RISC-V binary (machine type: %d)\n", fix_endian16(loader, header->e_machine));
            return false;
        }

        loader->entry_point = fix_endian64(loader, header->e_entry);
        return parse_program_headers_64(loader, header);
    } else {
        if (loader->file_size < sizeof(Elf32_Ehdr)) {
            fprintf(stderr, "File too small for 32-bit ELF header\n");
            return false;
        }

        Elf32_Ehdr* header = (Elf32_Ehdr*)loader->file_data;

        if (fix_endian16(loader, header->e_machine) != EM_RISCV) {
            fprintf(stderr, "Not a RISC-V binary (machine type: %d)\n", fix_endian16(loader, header->e_machine));
            return false;
        }

        loader->entry_point = fix_endian32(loader, header->e_entry);
        return parse_program_headers_32(loader, header);
    }
}

// Main load function
static bool
elf_loader_load(RiscvElfLoader* loader)
{
    if (!load_file(loader)) { return false; }

    if (!validate_elf_header(loader)) { return false; }

    if (!parse_elf_header(loader)) { return false; }

    return true;
}

// Get entry point
static uint64_t
elf_loader_get_entry_point(const RiscvElfLoader* loader)
{
    return loader->entry_point;
}

// Read memory from loaded segments
static bool
elf_loader_read_memory(const RiscvElfLoader* loader, uint64_t address, void* buffer, size_t size)
{
    for (int i = 0; i < loader->num_segments; i++) {
        const MemorySegment* segment = &loader->segments[i];
        if (address >= segment->vaddr && address + size <= segment->vaddr + segment->size) {
            size_t offset = address - segment->vaddr;
            memcpy(buffer, segment->data + offset, size);
            return true;
        }
    }
    return false;
}

// Get segment information
static int
elf_loader_get_num_segments(const RiscvElfLoader* loader)
{
    return loader->num_segments;
}

static const MemorySegment*
elf_loader_get_segment(const RiscvElfLoader* loader, int index)
{
    if (index < 0 || index >= loader->num_segments) { return NULL; }
    return &loader->segments[index];
}

// Print loader information
static void
elf_loader_print_info(const RiscvElfLoader* loader)
{
    printf("ELF File: %s\n", loader->filename);
    printf("Class: %s\n", loader->is_64bit ? "64-bit" : "32-bit");
    printf("Endianness: %s\n", loader->is_little_endian ? "Little" : "Big");
    printf("Machine: RISC-V\n");
    printf("Entry point: 0x%llx\n", loader->entry_point);
    printf("Loadable segments: %d\n", loader->num_segments);

    printf("\nMemory segments:\n");
    for (int i = 0; i < loader->num_segments; i++) {
        const MemorySegment* seg      = &loader->segments[i];
        char                 flags[4] = { 0 };
        int                  flag_idx = 0;

        if (seg->flags & PF_R) flags[flag_idx++] = 'R';
        if (seg->flags & PF_W) flags[flag_idx++] = 'W';
        if (seg->flags & PF_X) flags[flag_idx++] = 'X';

        printf("  Segment %d: VA=0x%llx-0x%llx, size=0x%llx, flags=%s\n",
               i,
               seg->vaddr,
               seg->vaddr + seg->size,
               seg->size,
               flags);
    }
}

XP_EMULATOR_EXTERN
RiscvElfLoader*
xp_emulator_elf_loader_load(const char* program_binary)
{
    RiscvElfLoader* loader = elf_loader_create(program_binary);

    if (!loader) {
        fprintf(stderr, "Failed to create ELF loader\n");
        return NULL;
    }

    if (!elf_loader_load(loader)) {
        fprintf(stderr, "Failed to load ELF file\n");
        elf_loader_destroy(loader);
        return NULL;
    }

    elf_loader_print_info(loader);

    // Example: read first 16 bytes from entry point
    uint64_t entry = elf_loader_get_entry_point(loader);
    if (entry != 0) {
        uint8_t data[16];
        if (elf_loader_read_memory(loader, entry, data, 16)) {
            printf("\nFirst 16 bytes at entry point (0x%llx):\n", entry);
            for (int i = 0; i < 16; i++) { printf("%02x ", data[i]); }
            printf("\n");
        }
    }

    return loader;
}

XP_EMULATOR_EXTERN void
xp_emulator_elf_loader_unload(RiscvElfLoader* loader)
{
    elf_loader_destroy(loader);
}
