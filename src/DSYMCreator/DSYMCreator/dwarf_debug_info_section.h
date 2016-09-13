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
#include <vector>
#include "symbol.h"

struct DwarfDebugInfoSection {
    
    std::vector<Symbol> symbols;
    
    std::vector<uint8_t> dump(const std::map<std::string, uint32_t>& name_to_offset) const;
};

#endif /* dwarf_debug_info_section_h */
