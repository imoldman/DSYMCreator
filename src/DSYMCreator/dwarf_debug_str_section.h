//
//  dwarf_debug_str_section.h
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#ifndef dwarf_debug_str_section_h
#define dwarf_debug_str_section_h

#include <string>
#include <vector>
#include <map>

struct DwarfDebugStrSection {
    struct DumpResult {
        std::vector<uint8_t> buffer;
        std::map<std::string, uint32_t> name_to_offset;
    };
    std::vector<std::string> names;
    
    DumpResult dump() const;
};

#endif /* dwarf_debug_str_section_h */
