//
//  dwarf_dummy_debug_line_section.c
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#include "dwarf_dummy_debug_line_section.h"

#include <vector>

// modified from lldb project, DWARFDebugLine.h/cpp
struct DWARFDebugLine {
    uint32_t    total_length;
    uint16_t    version;
    uint32_t    prologue_length;
    uint8_t     min_inst_length;
    uint8_t     default_is_stmt;
    int8_t      line_base;
    uint8_t     line_range;
    uint8_t     opcode_base;
};

std::vector<uint8_t> DwarfDummyDebugLineSection::dump() const {
    
    // simply, we can only dump an empty debug line section to cheat lldb,
    // but an empty section makes lldb log some noisy message in stdout,
    // so here we make a normal debug line section, but without any data
    DWARFDebugLine section;
    section.total_length = sizeof(DWARFDebugLine) - sizeof(uint32_t);
    section.version = 2;
    section.prologue_length = section.total_length - sizeof(uint32_t) - sizeof(uint16_t);
    section.min_inst_length = 1;
    section.default_is_stmt = 1;
    section.line_base = 0;
    section.line_range = 0;
    section.opcode_base = 1;
    
    std::vector<uint8_t> result((uint8_t*)(&section), (uint8_t*)(&section+1));
    return result;
}