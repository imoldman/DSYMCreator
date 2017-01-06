//
//  dwarf_debug_info_section.h
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#ifndef dwarf_debug_info_section_h
#define dwarf_debug_info_section_h

#include <map>

#include "common.h"
#include "dwarf.h"
#include "symbol.h"
#include "util.h"

template <typename T>
struct DwarfDebugInfoSection {
    ByteBuffer dump(const std::vector<Symbol<T>>& symbols,
                    const std::map<std::string, uint32_t>& name_to_offset) const;
};

// see DWARFDebugInfoEntry.cpp in lldb project
template <typename T>
ByteBuffer DwarfDebugInfoSection<T>::dump(const std::vector<Symbol<T>>& symbols, const std::map<std::string, uint32_t>& name_to_offset) const
{
    // since all the value we used can be narrowed to one byte,
    // so we don't need encode data with LEB128, see DataExtractor.cpp in lldb project
    ByteBuffer buffer;
    
    // header
    util::append_to_buffer<uint32_t>(buffer, 0); // length, will be filled at the end of this function;
    util::append_to_buffer<uint16_t>(buffer, 2); // version
    util::append_to_buffer<uint32_t>(buffer, 0); // abbr_offset
    util::append_to_buffer<uint8_t>(buffer, sizeof(T));  // addr_size
    
    // compile unit header
    util::append_to_buffer<uint8_t>(buffer, 1);                    // index in abbrev section
    util::append_to_buffer<uint16_t>(buffer, DW_LANG_ObjC);        // language
    
    // functions
    for (const auto& s: symbols) {
        util::append_to_buffer<uint8_t>(buffer, 2);        // index in abbrev section
        util::append_to_buffer<T>(buffer, s.base);          // start address
        util::append_to_buffer<T>(buffer, s.end);           // end address
        auto it = name_to_offset.find(s.name);
        assert(it != name_to_offset.end());
        util::append_to_buffer<uint32_t>(buffer, it->second);   // name offset in __dwarf_str section
    }
    
    util::append_to_buffer<uint8_t>(buffer, 0);        // empty compile uint header
    
    // fill length
    *(uint32_t*)(&buffer[0]) = (uint32_t)(buffer.size() - sizeof(uint32_t));
    
    return buffer;
}

#endif /* dwarf_debug_info_section_h */
