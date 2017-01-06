//
//  string_table.h
//  DSYMCreator
//
//  Created by oldman on 8/10/16.
//
//

#ifndef __DSYMCreatore__string_table__
#define __DSYMCreatore__string_table__

#include <assert.h>
#include <map>
#include <string>
#include <vector>

#include "common.h"

struct StringTable {
    struct DumpResult {
        ByteBuffer buffer;
        std::map<std::string, uint32_t> name_to_offset;
    };
    DumpResult dump(const std::vector<std::string>& names) const;
};

#endif /* __DSYMCreatore__string_table__ */
