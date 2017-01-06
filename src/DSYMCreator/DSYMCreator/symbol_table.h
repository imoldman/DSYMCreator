//
//  symbol_table.h
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
//
//

#ifndef symbol_table_h
#define symbol_table_h

#include <type_traits>
#include <vector>

#include "common.h"
#include "macho_type_wrapper.h"
#include "util.h"
#include "string_table.h"
#include "symbol.h"


template <typename T>
struct SymbolTable {
    ByteBuffer dump(const std::vector<Symbol<T>>& symbols,
                    const std::map<std::string, uint32_t>& name_to_offset) const;
};

template <typename T>
ByteBuffer SymbolTable<T>::dump(const std::vector<Symbol<T>>& symbols,
                                const std::map<std::string, uint32_t>& name_to_offset) const{
    ByteBuffer result;
    for (const auto& s: symbols) {
        Nlist<T> nlist;
        auto it = name_to_offset.find(s.name);
        assert(it != name_to_offset.end());
        nlist.n_un.n_strx = it->second;
        nlist.n_type = N_SECT;
        nlist.n_sect = 1;      // since we only have one dummy section
        nlist.n_desc = std::is_same<T, uint64_t>::value ? 0 : N_ARM_THUMB_DEF;      // under 64 bit, there is no thumb mode
        nlist.n_value = s.base;
        util::append_to_buffer(result, nlist);
    }
    
    return result;
}

#endif /* symbol_table_h */
