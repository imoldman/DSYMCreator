//
//  Created by oldman on 6/8/15.
//


#include "util.h"
#include "dwarf_macho.h"
#include <assert.h>

#import <Foundation/Foundation.h>

static uint32_t hexStringToInt(NSString* string) {
    NSScanner* scanner = [NSScanner scannerWithString:string];
    unsigned int result = 0;
    [scanner scanHexInt:&result];
    return (uint32_t)result;
}

int main(int argc, char* argv[]) {
    DwarfMacho dwarf;
//    dwarf.uuid = "14494083A18431E2946B3F942A402952"; // it's LearnBlock
//    dwarf.uuid = "19F6C507AF6C3426BC7D9E9FDBD429CB"; // it's Spark
    
    {
        NSError* error = nil;
        NSString* content = [NSString stringWithContentsOfFile:@"/Users/oldman/Desktop/DSYMCreator/example2.txt" encoding:NSUTF8StringEncoding error:&error];
        assert(error == nil);
        NSArray* lines = [content componentsSeparatedByString:@"\n"];
        for (NSString* line in lines) {
            if (line.length > 0) {
                NSArray* parts = [line componentsSeparatedByString:@"\t"];
                assert(parts.count == 3);
                std::string name([parts[2] UTF8String]);
                uint32_t base = hexStringToInt(parts[0]);
                uint32_t end = hexStringToInt(parts[1]);
                dwarf.symbols.push_back(Symbol(name, base, end));
            }
        }
    }
    
//    dwarf.symbols.push_back(Symbol("-[AppDelegate applicationDidEnterBackground:]", 0xBD2A, 0xBD88));
//    dwarf.symbols.push_back(Symbol("_main", 0xC4D6, 0xC532));
    auto buffer = dwarf.dump();
    util::write("/Users/oldman/Desktop/DSYMCreator/output.dwarf.armv7", 0, (uint32_t)buffer.size(), &buffer[0]);
    
}
