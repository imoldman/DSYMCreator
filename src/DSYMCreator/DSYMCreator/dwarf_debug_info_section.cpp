//
//  dwarf_debug_info_section.c
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#include "dwarf_debug_info_section.h"

#include <assert.h>
#include "dwarf.h"

template <typename T>
static void push_back_to_buffer(std::vector<uint8_t>& buffer, T t) {
    buffer.insert(buffer.end(), (uint8_t*)&t, (uint8_t*)(&t+1));
}

// see DWARFDebugInfoEntry.cpp in lldb project
std::vector<uint8_t> DwarfDebugInfoSection::dump(const std::map<std::string, uint32_t>& name_to_offset) const {
    
    // since all the value we used can be narrowed to one byte,
    // so we don't need encode data with LEB128, see DataExtractor.cpp in lldb project
    std::vector<uint8_t> buffer;
    
    // header
    push_back_to_buffer(buffer, (uint32_t)0); // length, will be filled at the end of this function;
    push_back_to_buffer(buffer, (uint16_t)2); // version
    push_back_to_buffer(buffer, (uint32_t)0); // abbr_offset
    push_back_to_buffer(buffer, (uint8_t)4);  // addr_size
    
    // compile unit header
    push_back_to_buffer(buffer, (uint8_t)1);                    // index in abbrev section
    push_back_to_buffer(buffer, (uint16_t)DW_LANG_ObjC);        // language
    
    // functions
    for (const auto& s: symbols) {
        push_back_to_buffer(buffer, (uint8_t)2);        // index in abbrev section
        push_back_to_buffer(buffer, (uint32_t)(s.base));
        push_back_to_buffer(buffer, (uint32_t)(s.end));
        auto it = name_to_offset.find(s.name);
        assert(it != name_to_offset.end());
        push_back_to_buffer(buffer, (uint32_t)it->second);
    }
    
    push_back_to_buffer(buffer, (uint8_t)0);        // empty compile uint header
    
    // fill length
    *(uint32_t*)(&buffer[0]) = (uint32_t)(buffer.size() - sizeof(uint32_t));
    
    return buffer;
}