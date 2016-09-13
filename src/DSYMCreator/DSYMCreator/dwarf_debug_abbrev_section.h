//
//  dwarf_debug_abbrev_section.h
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#ifndef dwarf_debug_abbrev_section_h
#define dwarf_debug_abbrev_section_h

#include <vector>
#include "symbol.h"

struct DwarfDebugAbbrevSection {
    
    std::vector<Symbol> symbols;
    
    std::vector<uint8_t> dump() const;
};

#endif /* dwarf_debug_abbrev_section_h */
