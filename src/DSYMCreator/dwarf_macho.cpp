//
//  dwarf_macho.cpp
//  DSYMCreator
//
//  Created by oldman on 8/10/16.
//
//

#include "dwarf_macho.h"

#include <assert.h>
#include <algorithm>
#include "mach-o/loader.h"
#include "dwarf_debug_str_section.h"
#include "dwarf_debug_abbrev_section.h"
#include "dwarf_debug_info_section.h"
#include "dwarf_dummy_debug_line_section.h"
#include "symbol_table.h"
#include "string_table.h"
#include "util.h"

struct MachHeader : public mach_header {
    MachHeader() {
        magic = MH_MAGIC;
        cputype = CPU_TYPE_ARM;
        cpusubtype = CPU_SUBTYPE_ARM_V7;
        filetype = MH_DSYM;
        flags = 0;
    }
};
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
struct TextSegmentCommand : public segment_command {
    TextSegmentCommand() {
        cmd = LC_SEGMENT;
        memset(segname, 0, sizeof(segname));
        memcpy(segname, "__TEXT", 6);
        vmaddr = 0x4000;       // text segment base address, must equal to the value in binary, TODO: detect by outside
        vmsize = 0;            // no need to fill, since lldb doesn't use it
        fileoff = 0;
        filesize = 0;
        maxprot = VM_PROT_READ | VM_PROT_EXECUTE;
        initprot = VM_PROT_READ | VM_PROT_EXECUTE;
        flags = 0;
    }
};

struct TextSectionHeader : public section {
    TextSectionHeader() {
        memset(sectname, 0, sizeof(sectname));
        memcpy(sectname, "__text", 6);
        memset(segname, 0, sizeof(segname));
        memcpy(segname, "__TEXT", 6);
        addr = 0;       // no need to fill, since lldb doesn't use it
        size = 0;       // no need to fill, since lldb doesn't use it
        offset = 0;
        align = 0x3;
        reloff = 0;
        nreloc = 0;
        flags = 0;      // no need to fill, since lldb doesn't use it
        reserved1 = 0;
        reserved2 = 0;
    }
};

struct DwarfSegmentCommand : public segment_command {
    DwarfSegmentCommand() {
        cmd = LC_SEGMENT;
        memset(segname, 0, sizeof(segname));
        memcpy(segname, "__DWARF", 7);
        vmsize = 0;            // no need to fill, since lldb doesn't use it
        maxprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE;
        initprot = VM_PROT_READ | VM_PROT_WRITE;
        flags = 0;
    }
};

struct DwarfCommonSectionHeader : public section {
    DwarfCommonSectionHeader(const std::string& section_name, uint32_t vmbase, uint32_t file_offset, uint32_t length) {
        memset(sectname, 0, sizeof(sectname));
        memcpy(sectname, section_name.c_str(), section_name.size());
        memset(segname, 0, sizeof(segname));
        memcpy(segname, "__DWARF", 7);
        addr = vmbase + file_offset;
        size = length;
        offset = file_offset;
        align = 0;
        reloff = 0;
        nreloc = 0;
        flags = 0;
        reserved1 = 0;
        reserved2 = 0;
    }
};

template <typename T>
void appendToBuffer(std::vector<uint8_t>& buffer, const T& t) {
    size_t origin_size = buffer.size();
    buffer.resize(origin_size + sizeof(t));
    memcpy(&buffer[origin_size], &t, sizeof(t));
}

void appendBuffer(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& buffer2) {
    size_t origin_size = buffer.size();
    buffer.resize(origin_size + buffer2.size());
    memcpy(&buffer[origin_size], &buffer2[0], buffer2.size());
}

std::vector<uint8_t> DwarfMacho::dump() const {
    
    // prepare strings
    std::vector<std::string> names;
    for (const auto& s: symbols) {
        names.push_back(s.name);
    }
    
    // prepare string table
    StringTable string_table;
    string_table.names = names;
    StringTable::DumpResult string_result = string_table.dump();
    
    // preapre symbol table
    SymbolTable symbol_table;
    symbol_table.symbols = symbols;
    SymbolTable::DumpResult symbol_result = symbol_table.dump(string_result.name_to_offset);
    
    // prepare dwarf sections
    DwarfDummyDebugLineSection debug_line_section;
    auto debug_line_buffer = debug_line_section.dump();
    DwarfDebugStrSection debug_str_section;
    debug_str_section.names = names;
    auto debug_str_dump_result = debug_str_section.dump();
    DwarfDebugAbbrevSection debug_abbrev_section;
    debug_abbrev_section.symbols = symbols;
    auto debug_abbrev_buffer = debug_abbrev_section.dump();
    DwarfDebugInfoSection debug_info_section;
    debug_info_section.symbols = symbols;
    auto debug_info_buffer = debug_info_section.dump(debug_str_dump_result.name_to_offset);
    
    // prepare symtab command
    SymtabCommand symtab_command;
    symtab_command.symoff = 0x1000;
    symtab_command.nsyms = uint32_t(symbols.size());
    symtab_command.stroff = uint32_t(symtab_command.symoff + symbol_result.buffer.size());
    symtab_command.strsize = uint32_t(string_result.buffer.size());
    
    // prepare text segment command
    TextSectionHeader text_section_header;
    TextSegmentCommand text_segment_command;
    text_segment_command.nsects = 1;
    text_segment_command.cmdsize = sizeof(TextSectionHeader) + sizeof(TextSegmentCommand);
    
    // prepare dwarf segement command
    uint32_t vmbase = 0x300000;         // TODO: choose a safe vmbase and start offset
    uint32_t start_offset = 0x300000;
    uint32_t offset = start_offset;
    DwarfCommonSectionHeader debug_line_section_header("__debug_line", vmbase, offset, (uint32_t)debug_line_buffer.size());
    offset += debug_line_buffer.size();
    DwarfCommonSectionHeader debug_info_section_header("__debug_info", vmbase, offset, (uint32_t)debug_info_buffer.size());
    offset += debug_info_buffer.size();
    DwarfCommonSectionHeader debug_abbrev_section_header("__debug_abbrev", vmbase, offset, (uint32_t)debug_abbrev_buffer.size());
    offset += debug_abbrev_buffer.size();
    DwarfCommonSectionHeader debug_str_section_header("__debug_str", vmbase, offset, (uint32_t)debug_str_dump_result.buffer.size());
    offset += debug_str_dump_result.buffer.size();

    DwarfSegmentCommand dwarf_segment_command;
    dwarf_segment_command.vmaddr = vmbase + start_offset;
    dwarf_segment_command.fileoff = start_offset;
    dwarf_segment_command.filesize = offset - start_offset;
    dwarf_segment_command.nsects = 4;
    dwarf_segment_command.cmdsize = sizeof(DwarfSegmentCommand) + 4 * sizeof(DwarfCommonSectionHeader);
    
    // prepare uuid command
    UUIDCommand uuid_command;
    std::string clean_uuid = uuid;
    clean_uuid.erase(std::remove(clean_uuid.begin(), clean_uuid.end(), '-'), clean_uuid.end());     // remove the hyphen in uuid first
    assert(clean_uuid.length() == 32);
    for (int i = 0; i < 16; ++i) {
        std::string str = clean_uuid.substr(2*i, 2);
        uuid_command.uuid[i] = strtol(str.c_str(), NULL, 16);
    }
    
    // prepare mach header
    MachHeader mach_header;
    mach_header.ncmds = 3;
    mach_header.sizeofcmds = sizeof(UUIDCommand) + sizeof(SymtabCommand) + sizeof(TextSegmentCommand) + sizeof(TextSectionHeader);
    
    // write
    std::vector<uint8_t> buffer;
    appendToBuffer(buffer, mach_header);
    appendToBuffer(buffer, uuid_command);
    appendToBuffer(buffer, symtab_command);
    appendToBuffer(buffer, text_segment_command);
    appendToBuffer(buffer, text_section_header);
//    appendToBuffer(buffer, dwarf_segment_command);
//    appendToBuffer(buffer, debug_line_section_header);
//    appendToBuffer(buffer, debug_str_section_header);
//    appendToBuffer(buffer, debug_abbrev_section_header);
//    appendToBuffer(buffer, debug_info_section_header);
    assert(buffer.size() <= 0x1000);
    buffer.resize(0x1000);
    appendBuffer(buffer, symbol_result.buffer);
    appendBuffer(buffer, string_result.buffer);
//    assert(buffer.size() <= 0x300000);
//    buffer.resize(0x300000);
//    appendBuffer(buffer, debug_line_buffer);
//    appendBuffer(buffer, debug_info_buffer);
//    appendBuffer(buffer, debug_abbrev_buffer);
//    appendBuffer(buffer, debug_str_dump_result.buffer);

    return buffer;
}


