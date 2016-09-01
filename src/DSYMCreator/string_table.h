//
//  string_table.h
//  DSYMCreator
//
//  Created by oldman on 8/10/16.
//
//

#ifndef __DSYMCreatore__string_table__
#define __DSYMCreatore__string_table__

#include <map>
#include <vector>
#include <string>

struct StringTable {
    struct DumpResult {
        std::vector<uint8_t> buffer;
        std::map<std::string, uint32_t> name_to_offset;
    };
    std::vector<std::string> names;
    DumpResult dump() const;
};

#endif /* __DSYMCreatore__string_table__ */
