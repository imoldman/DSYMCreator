//
//  symbol_table.h
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
//
//

#ifndef symbol_table_h
#define symbol_table_h

#include <vector>
#include "nlist.h"
#include "string_table.h"
#include "symbol.h"

struct Nlist : public nlist {};
struct Nlist64 : public nlist_64 {};

struct SymbolTable {
    struct DumpResult {
        std::vector<uint8_t> buffer;
    };
    
    std::vector<Symbol> symbols;
    DumpResult dump(const std::map<std::string, uint32_t>& name_to_offset) const;
};


#endif /* symbol_table_h */
