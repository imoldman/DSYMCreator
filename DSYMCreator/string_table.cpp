//
//  string_table.cpp
//  DSYMCreator
//
//  Created by oldman on 8/31/16.
//
//

#include "string_table.h"

#include <assert.h>

StringTable::DumpResult StringTable::dump() const{
    
    uint32_t needed_length = 0;
    for (const auto& n: names) {
        needed_length += n.size() + 1;  // don't forget trailing zero
    }
    
    StringTable::DumpResult result;
    result.buffer.resize(needed_length);
    
    std::uint32_t current_offset = 0;
    for (const auto& n: names) {
        result.name_to_offset[n] = current_offset;
        memcpy(&result.buffer[current_offset], &n[0], n.length());
        current_offset += n.length();
        current_offset++;       // skip trailing zero
    }
    
    assert(current_offset == needed_length);
    
    return result;
}