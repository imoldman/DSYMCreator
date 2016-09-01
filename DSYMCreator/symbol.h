//
//  symbol.h
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
//
//

#ifndef symbol_h
#define symbol_h

#include <string>

struct Symbol {
    std::string name;
    uint32_t base;      // base address
    uint32_t end;       // end address
    
    Symbol(std::string name, uint32_t base, uint32_t end) : name(name), base(base), end(end) {}
};

#endif /* symbol_h */
