//
//  symbol_table.cpp
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
//
//

#include "symbol_table.h"

#include <assert.h>

SymbolTable::DumpResult SymbolTable::dump(const std::map<std::string, uint32_t>& name_to_offset) const{
    
    SymbolTable::DumpResult result;
    uint32_t needed_length = uint32_t(sizeof(Nlist) * symbols.size());
    result.buffer.resize(needed_length);
    
    uint32_t current_offset = 0;
    for (const auto& s: symbols) {
        Nlist* nlist = (Nlist*)(&result.buffer[current_offset]);
        auto it = name_to_offset.find(s.name);
        assert(it != name_to_offset.end());
        nlist->n_un.n_strx = it->second;
        nlist->n_type = N_SECT;
        nlist->n_sect = 1;      // since we only have one dummy section
        nlist->n_desc = N_ARM_THUMB_DEF;
        nlist->n_value = s.base;
        current_offset += sizeof(Nlist);
    }
    
    assert(current_offset == needed_length);
    return result;
}