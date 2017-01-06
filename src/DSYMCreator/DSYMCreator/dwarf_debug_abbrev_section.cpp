//
//  dwarf_debug_abbrev_section.c
//  DSYMCreator
//
//  Created by oldman on 9/1/16.
//
//

#include "dwarf_debug_abbrev_section.h"

#include <assert.h>
#include "dwarf.h"

// dump key value in abbrev section
ByteBuffer DwarfDebugAbbrevSection::dump() const {
    // since all the value we used can be narrowed to one byte,
    // so we don't need encode data with LEB128, see DataExtractor.cpp in lldb project
    ByteBuffer result;
    
    // header
    result.push_back(1);        // index
    result.push_back(DW_TAG_compile_unit), result.push_back(DW_CHILDREN_yes);
    result.push_back(DW_AT_language), result.push_back(DW_FORM_data2);      // source code language
    result.push_back(0), result.push_back(0);     // end flag
    
    // content, all the content use the same layout
    result.push_back(2);        // index
    result.push_back(DW_TAG_subprogram), result.push_back(DW_CHILDREN_no);
    result.push_back(DW_AT_low_pc), result.push_back(DW_FORM_addr);
    result.push_back(DW_AT_high_pc), result.push_back(DW_FORM_addr);
    result.push_back(DW_AT_name), result.push_back(DW_FORM_strp);
    result.push_back(0), result.push_back(0);     // end flag
    
    return result;
}
