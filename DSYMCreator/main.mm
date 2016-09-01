//
//  Created by oldman on 6/8/15.
//


#include "util.h"
#include "dwarf_macho.h"

int main(int argc, char* argv[]) {
    DwarfMacho dwarf;
    dwarf.uuid = "14494083A18431E2946B3F942A402952";
    
    
    
    dwarf.symbols.push_back(Symbol("-[AppDelegate applicationDidEnterBackground:]", 0xBD2A, 0xBD88));
    dwarf.symbols.push_back(Symbol("_main", 0xC4D6, 0xC532));
    auto buffer = dwarf.dump();
    util::write("/Users/oldman/Desktop/hack/LearnBlock.app.dSYM/Contents/Resources/DWARF/LearnBlock.DWARF.test", 0, (uint32_t)buffer.size(), &buffer[0]);
    
    {
//        std::string output = "/Users/oldman/Desktop/hack/LearnBlock.app.dSYM/Contents/Resources/DWARF/LearnBlock.DWARF.test";
//        std::string input = "/Users/oldman/Desktop/hack/LearnBlock.app.dSYM/Contents/Resources/DWARF/LearnBlock.DWARF.armv7.origin.armv7";
//        
//        auto origin_buffer = buffer;
//        uint32_t start = 0;
//        uint32_t end = 0x798;
//        uint32_t length = end - start;
//        uint32_t command_count = 5;
//        std::vector<uint8_t> buffer(length);
//        util::read(input, start, length, &buffer[0]);
//        *(uint32_t*)(&buffer[0x10]) = command_count;
//        *(uint32_t*)(&buffer[0x14]) = 0x798 - 0x1c;
//        memcpy(&buffer[0x3c], &origin_buffer[0x3c], 0x10);
//        util::write(output, start, length, &buffer[0]);
        
    }
}
