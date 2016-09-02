//
//  Created by oldman on 6/8/15.
//

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <gflags/gflags.h>
#import <Foundation/Foundation.h>

#include "exception.h"
#include "util.h"
#include "dwarf_macho.h"


DEFINE_string(uuid, "", "uuid for application binary");
DEFINE_string(raw_ida_symbol, "", "file path for the output by the ida script");
DEFINE_string(output, "", "file path to save the symbol");

static void init_gflag_config(int& argc, char**& argv) {
    std::string usage("recreate the symbol file from ida output for ios application, e.g.\n");
    usage += std::string("\t") + argv[0] + " --uuid \"14494083-a184-31e2-946b-3f942a402952\" --raw_ida_symbol \"/tmp/symbols.txt\" --output \"/path/to/save/loadable_symbol\" \n";
    usage += std::string("then if no error occurs, a fresh symbol file will be created at /path/to/save/loadable_symbol");
    ::google::SetUsageMessage(usage);
    
    ::google::ParseCommandLineFlags(&argc, &argv, true);
    
    if (FLAGS_uuid.length() == 0 || FLAGS_raw_ida_symbol.length() == 0 || FLAGS_output.length() == 0) {
        throw Exception(ExceptionCode::kParamInvalid, "invalid param, please check the usage");
    }
}

static uint32_t hexStringToInt(NSString* string) {
    NSScanner* scanner = [NSScanner scannerWithString:string];
    unsigned int result = 0;
    [scanner scanHexInt:&result];
    return (uint32_t)result;
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
            uint32_t base = hexStringToInt(parts[0]);
            uint32_t end = hexStringToInt(parts[1]);
            symbols.push_back(Symbol(name, base, end));
        }
    }
    return symbols;
}

static std::string build_success_result(const std::string& content) {
    return util::build_string([&](std::ostringstream& ss) {
        ss << "{\"error\":0, \"decription\":\"" << content << "\"}";
    });
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
        auto buffer = dwarf.dump();
        util::write(FLAGS_output, 0, (uint32_t)buffer.size(), &buffer[0]);
        std::cout << build_success_result("create symbol file success") << std::endl;
        return 0;
    } catch (Exception& e) {
        std::cerr << build_failed_result(e) << std::endl;
        return (int)e.code();
    }
}
