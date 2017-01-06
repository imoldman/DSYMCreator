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

template <typename T>
struct Symbol {
    std::string name;
    T base;      // base address
    T end;       // end address
    
    Symbol(std::string name, T base, T end) : name(name), base(base), end(end) {}
};

#endif /* symbol_h */
