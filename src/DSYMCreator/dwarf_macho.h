//
//  dwarf_macho.h
//  DSYMCreator
//
//  Created by oldman on 8/10/16.
//
//

#ifndef dwarf_macho_h
#define dwarf_macho_h

#include <vector>
#include <string>
#include "symbol.h"

struct DwarfMacho {
    
    std::string uuid;
    std::vector<Symbol> symbols;
    
    std::vector<uint8_t> dump(uint32_t section_vm_addr_offset) const;
};

#endif /* dwarf_macho_h */
