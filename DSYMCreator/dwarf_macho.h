//
//  dwarf_macho.h
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
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
    
    std::vector<uint8_t> dump() const;
};

#endif /* dwarf_macho_h */
