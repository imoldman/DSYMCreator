//
//  dwarf_dummy_debug_line_section.c
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#include "dwarf_dummy_debug_line_section.h"

#include <vector>
#include "macho_type_wrapper.h"
#include "util.h"

ByteBuffer DwarfDummyDebugLineSection::dump() const {
    // simply, we can only dump an empty debug line section to cheat lldb,
    // but an empty section makes lldb log some noisy message in stdout,
    // so here we make a normal debug line section, but without any data
    DwarfDebugLine section;
    memset(&section, 0, sizeof(section));        // DWARFDebugLine isn't align to uint32_t, so we clear it at first
    section.total_length = sizeof(DwarfDebugLine) - sizeof(DwarfDebugLine::total_length);
    section.version = 2;
    section.prologue_length = section.total_length - sizeof(DwarfDebugLine::version) - sizeof(DwarfDebugLine::prologue_length);
    section.min_inst_length = 1;
    section.default_is_stmt = 1;
    section.line_base = 0;
    section.line_range = 0;
    section.opcode_base = 1;
    
    ByteBuffer result;
    util::append_to_buffer(result, section);
    return result;
}
