//
//  Created by oldman on 6/8/15.
//

#import <Foundation/Foundation.h>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <gflags/gflags.h>

#include "exception.h"
#include "util.h"
#include "dwarf_macho.h"


DEFINE_string(uuid, "", "uuid for application binary");
DEFINE_string(raw_ida_symbol, "", "file path for the output by the ida script");
DEFINE_string(output, "", "file path to save the symbol");
DEFINE_string(dwarf_section_vmbase, "", "vm base addr for dwarf sections, in hex format");

static void init_gflag_config(int& argc, char**& argv) {
    std::string usage("recreate the symbol file from ida output for ios application, e.g.\n");
    usage += std::string("\t") + argv[0] + " --uuid \"14494083-a184-31e2-946b-3f942a402952\" --raw_ida_symbol \"/tmp/symbols.txt\" --dwarf_section_vmbase 0x40000 --output \"/path/to/save/loadable_symbol\" \n";
    usage += std::string("then if no error occurs, a fresh symbol file will be created at /path/to/save/loadable_symbol");
    ::google::SetUsageMessage(usage);
    
    ::google::ParseCommandLineFlags(&argc, &argv, true);
    
    if (FLAGS_uuid.length() == 0 ||
        FLAGS_raw_ida_symbol.length() == 0 ||
        FLAGS_dwarf_section_vmbase.length() == 0 ||
        FLAGS_output.length() == 0) {
        throw Exception(ExceptionCode::kParamInvalid, "invalid param, please check the usage");
    }
}

static std::vector<Symbol> readRawSymbolDataFromPath(const std::string& path) {
    NSError* error = nil;
    NSString* content = [NSString stringWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()] encoding:NSUTF8StringEncoding error:&error];
    assert(error == nil);
    NSArray* lines = [content componentsSeparatedByString:@"\n"];
    std::vector<Symbol> symbols;
    for (NSString* line in lines) {
        if (line.length > 0) {
            NSArray* parts = [line componentsSeparatedByString:@"\t"];
            assert(parts.count == 3);
            std::string name([parts[2] UTF8String]);
            uint32_t base = (uint32_t)std::stoul(std::string([parts[0] UTF8String]), nullptr, 16);
            uint32_t end = (uint32_t)std::stoul(std::string([parts[1] UTF8String]), nullptr, 16);;
            symbols.push_back(Symbol(name, base, end));
        }
    }
    return symbols;
}

static std::string build_failed_result(const Exception& exception) {
    return util::build_string([&](std::ostringstream& ss) {
        ss << "{\"error\":" << (int)exception.code() << ", \"description\":\"" << exception.description() << "\"}";
    });
}

int main(int argc, char* argv[]) {
    
    try {
        init_gflag_config(argc, argv);
        
        DwarfMacho dwarf;
        dwarf.uuid = FLAGS_uuid;
        dwarf.symbols = readRawSymbolDataFromPath(FLAGS_raw_ida_symbol);
        auto buffer = dwarf.dump((uint32_t)std::stoul(FLAGS_dwarf_section_vmbase, nullptr, 16));
        util::write(FLAGS_output, 0, (uint32_t)buffer.size(), &buffer[0]);
        std::cout << "create symbol file success: " << FLAGS_output << std::endl;
        return 0;
    } catch (Exception& e) {
        std::cerr << build_failed_result(e) << std::endl;
        return (int)e.code();
    }
}
