//
//  macho_type_wrapper.h
//  DSYMCreator
//
//  Created by oldman on 1/6/17.
//
//

#ifndef macho_type_wrapper_h
#define macho_type_wrapper_h

#include <string>
#include <type_traits>
#include "mach-o/loader.h"
#include "nlist.h"

namespace internal {
    struct Nlist32 : public nlist {};
    struct Nlist64 : public nlist_64 {};
}

template <typename T>
using Nlist = typename std::conditional<
    std::is_same<T, uint64_t>::value,
    internal::Nlist64,
    internal::Nlist32
>::type;

namespace internal {
    struct MachHeader32 : public mach_header {
        MachHeader32() {
            magic = MH_MAGIC;
            cputype = CPU_TYPE_ARM;
            cpusubtype = CPU_SUBTYPE_ARM_V7;
            filetype = MH_DSYM;
            flags = 0;
        }
    };
    
    struct MachHeader64 : public mach_header_64 {
        MachHeader64() {
            magic = MH_MAGIC_64;
            cputype = CPU_TYPE_ARM64;
            cpusubtype = CPU_SUBTYPE_ARM64_ALL;
            filetype = MH_DSYM;
            flags = 0;
        }
    };
}

template <typename T>
using MachHeader = typename std::conditional<
    std::is_same<T, uint64_t>::value,
    internal::MachHeader64,
    internal::MachHeader32
>::type;

struct UUIDCommand : public uuid_command {
    UUIDCommand() {
        cmd = LC_UUID;
        cmdsize = 24;
    }
};

struct SymtabCommand : public symtab_command {
    SymtabCommand() {
        cmd = LC_SYMTAB;
        cmdsize = 24;
    }
};

namespace internal {
    struct SegmentCommand32 : public segment_command {};
    struct SegmentCommand64 : public segment_command_64 {};
    
    template <typename T>
    using SegmentCommand64Or32 = typename std::conditional<
        std::is_same<T, uint64_t>::value,
        SegmentCommand64,
        SegmentCommand32
    >::type;
    
    template <typename T>
    struct SegmentCommand : public SegmentCommand64Or32<T> {
        SegmentCommand() {
            this->cmd = std::is_same<T, uint64_t>::value ? LC_SEGMENT_64 : LC_SEGMENT;
            this->vmsize = 0;            // no need to fill, since lldb doesn't use it
            this->fileoff = 0;
            this->filesize = 0;
            this->initprot = VM_PROT_READ | VM_PROT_EXECUTE;
            this->maxprot = VM_PROT_READ | VM_PROT_EXECUTE;
            this->flags = 0;
        }
    };
}

template <typename T>
struct TextSegmentCommand : public internal::SegmentCommand<T> {
    TextSegmentCommand() {
        memset(this->segname, 0, sizeof(this->segname));
        memcpy(this->segname, "__TEXT", 6);
        // text segment base address, must equal to the value in binary, TODO: configurable
        this->vmaddr = std::is_same<T, uint64_t>::value ? 0x100000000 : 0x4000;
    }
};

template <typename T>
struct DwarfSegmentCommand : public internal::SegmentCommand<T> {
    DwarfSegmentCommand() {
        memset(this->segname, 0, sizeof(this->segname));
        memcpy(this->segname, "__DWARF", 7);
    }
};

namespace internal {
    struct Section32 : public section {};
    struct Section64 : public section_64 {};
    
    template <typename T>
    using Section64Or32 = typename std::conditional<
        std::is_same<T, uint64_t>::value,
        Section64,
        Section32
    >::type;
    
    template <typename T>
    struct Section : public Section64Or32<T> {
        Section() {
            this->flags = 0;
            this->reserved1 = 0;
            this->reserved2 = 0;
        }
    };
}

template <typename T>
struct TextSectionHeader : public internal::Section<T> {
    TextSectionHeader() {
        memset(this->sectname, 0, sizeof(this->sectname));
        memcpy(this->sectname, "__text", 6);
        memset(this->segname, 0, sizeof(this->segname));
        memcpy(this->segname, "__TEXT", 6);
        this->addr = 0;       // no need to fill, since lldb doesn't use it
        this->size = 0;       // no need to fill, since lldb doesn't use it
        this->offset = 0;
        this->align = 0x3;
        this->reloff = 0;
        this->nreloc = 0;
    }
};

template <typename T>
struct DwarfCommonSectionHeader : public internal::Section<T> {
    DwarfCommonSectionHeader(const std::string& section_name, T vmbase, uint32_t file_offset, T length) {
        memset(this->sectname, 0, sizeof(this->sectname));
        memcpy(this->sectname, section_name.c_str(), section_name.size());
        memset(this->segname, 0, sizeof(this->segname));
        memcpy(this->segname, "__DWARF", 7);
        this->addr = vmbase + file_offset;
        this->size = length;
        this->offset = file_offset;
        this->align = 0;
        this->reloff = 0;
        this->nreloc = 0;
    }
};

// modified from lldb project, DWARFDebugLine.h/cpp
struct DwarfDebugLine {
    uint32_t    total_length;
    uint16_t    version;
    uint32_t    prologue_length;
    uint8_t     min_inst_length;
    uint8_t     default_is_stmt;
    int8_t      line_base;
    uint8_t     line_range;
    uint8_t     opcode_base;
};

#endif /* macho_type_wrapper_h */
