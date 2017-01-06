//
//  dwarf_debug_abbrev_section.h
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#ifndef dwarf_debug_abbrev_section_h
#define dwarf_debug_abbrev_section_h

#include "common.h"

struct DwarfDebugAbbrevSection {
    ByteBuffer dump() const;
};

#endif /* dwarf_debug_abbrev_section_h */
