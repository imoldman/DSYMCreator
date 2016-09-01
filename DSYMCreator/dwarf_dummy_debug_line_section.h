//
//  dwarf_dummy_debug_line_section.h
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#ifndef dwarf_dummy_debug_line_section_h
#define dwarf_dummy_debug_line_section_h

#include<vector>

struct DwarfDummyDebugLineSection {
    std::vector<uint8_t> dump() const;
};

#endif /* dwarf_dummy_debug_line_section_h */
