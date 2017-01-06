//
//  macho.h
//  DSYMCreator
//
//  Created by oldman on 8/10/16.
//
//

#ifndef macho_h
#define macho_h

#include <assert.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>

#include "common.h"
#include "dwarf_debug_abbrev_section.h"
#include "dwarf_debug_info_section.h"
#include "dwarf_dummy_debug_line_section.h"
#include "symbol.h"
#include "symbol_table.h"
#include "string_table.h"
#include "util.h"

template <typename T>
struct Macho {
public:
    ByteBuffer dump(const std::string& uuid,
                    const std::vector<Symbol<T>>& symbols,
                    T section_vm_addr_offset) const;
    
private:
    template <typename U>
    static U align_to(U address, uint32_t align) {
        return ((address - 1) / align + 1) * align;
    }
    
    static const uint32_t kSymbolTableOffset = 0x1000;
    static const uint32_t kAlignBase = 0x1000;
};


template <typename T>
ByteBuffer Macho<T>::dump(const std::string& uuid,
                               const std::vector<Symbol<T>>& symbols,
                               T section_vm_addr_offset) const {
    // prepare strings
    std::vector<std::string> names;
    std::transform(symbols.begin(), symbols.end(), std::inserter(names, names.end()), [](const Symbol<T>& s) {
        return s.name;
    });
    
    // prepare string table
    StringTable string_table;
    auto string_result = string_table.dump(names);
    
    // preapre symbol table
    SymbolTable<T> symbol_table;
    auto symbol_buffer = symbol_table.dump(symbols, string_result.name_to_offset);
    
    // prepare dwarf sections
    DwarfDummyDebugLineSection debug_line_section;
    auto debug_line_buffer = debug_line_section.dump();
    DwarfDebugAbbrevSection debug_abbrev_section;
    auto debug_abbrev_buffer = debug_abbrev_section.dump();
    DwarfDebugInfoSection<T> debug_info_section;
    auto debug_info_buffer = debug_info_section.dump(symbols, string_result.name_to_offset);
    
    // prepare symtab command
    SymtabCommand symtab_command;
    symtab_command.symoff = kSymbolTableOffset;
    symtab_command.nsyms = uint32_t(symbols.size());
    symtab_command.stroff = uint32_t(symtab_command.symoff + symbol_buffer.size());
    symtab_command.strsize = uint32_t(string_result.buffer.size());
    
    // prepare text segment command
    TextSectionHeader<T> text_section_header;
    TextSegmentCommand<T> text_segment_command;
    text_segment_command.nsects = 1;
    text_segment_command.cmdsize = sizeof(TextSectionHeader<T>) + sizeof(TextSegmentCommand<T>);
    
    // prepare dwarf segement command
    T vmbase = align_to(section_vm_addr_offset, kAlignBase);
    uint32_t dwarf_sections_start_offset = align_to(symtab_command.stroff + symtab_command.strsize, kAlignBase);
    uint32_t offset = dwarf_sections_start_offset;
    DwarfCommonSectionHeader<T> debug_line_section_header("__debug_line", vmbase, offset, (uint32_t)debug_line_buffer.size());
    offset += debug_line_buffer.size();
    DwarfCommonSectionHeader<T> debug_info_section_header("__debug_info", vmbase, offset, (uint32_t)debug_info_buffer.size());
    offset += debug_info_buffer.size();
    DwarfCommonSectionHeader<T> debug_abbrev_section_header("__debug_abbrev", vmbase, offset, (uint32_t)debug_abbrev_buffer.size());
    offset += debug_abbrev_buffer.size();
    DwarfCommonSectionHeader<T> debug_str_section_header("__debug_str", vmbase, offset, (uint32_t)string_result.buffer.size());
    offset += string_result.buffer.size();
    
    DwarfSegmentCommand<T> dwarf_segment_command;
    dwarf_segment_command.vmaddr = vmbase + dwarf_sections_start_offset;
    dwarf_segment_command.fileoff = dwarf_sections_start_offset;
    dwarf_segment_command.filesize = offset - dwarf_sections_start_offset;
    dwarf_segment_command.nsects = 4;
    dwarf_segment_command.cmdsize = sizeof(DwarfSegmentCommand<T>) + 4 * sizeof(DwarfCommonSectionHeader<T>);
    
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
    MachHeader<T> mach_header;
    mach_header.ncmds = 4;
    mach_header.sizeofcmds = sizeof(UUIDCommand) + sizeof(SymtabCommand) + sizeof(TextSegmentCommand<T>) + sizeof(TextSectionHeader<T>);
    
    // write
    ByteBuffer buffer;
    util::append_to_buffer(buffer, mach_header);
    util::append_to_buffer(buffer, uuid_command);
    util::append_to_buffer(buffer, symtab_command);
    util::append_to_buffer(buffer, text_segment_command);
    util::append_to_buffer(buffer, text_section_header);
    util::append_to_buffer(buffer, dwarf_segment_command);
    util::append_to_buffer(buffer, debug_line_section_header);
    util::append_to_buffer(buffer, debug_str_section_header);
    util::append_to_buffer(buffer, debug_abbrev_section_header);
    util::append_to_buffer(buffer, debug_info_section_header);
    assert(buffer.size() <= kSymbolTableOffset);
    buffer.resize(kSymbolTableOffset);
    util::append_to_buffer(buffer, symbol_buffer);
    util::append_to_buffer(buffer, string_result.buffer);
    assert(buffer.size() <= dwarf_sections_start_offset);
    buffer.resize(dwarf_sections_start_offset);
    util::append_to_buffer(buffer, debug_line_buffer);
    util::append_to_buffer(buffer, debug_info_buffer);
    util::append_to_buffer(buffer, debug_abbrev_buffer);
    util::append_to_buffer(buffer, string_result.buffer);
    
    return buffer;
}




#endif /* macho_h */
