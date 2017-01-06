//
//  Created by oldman on 6/8/15.
//

#import <Foundation/Foundation.h>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <gflags/gflags.h>

#include "common.h"
#include "exception.h"
#include "util.h"
#include "macho.h"

DEFINE_string(uuid, "", "uuid for application binary");
DEFINE_string(raw_ida_symbol, "", "file path for the output by the ida script");
DEFINE_string(output, "", "file path to save the symbol");
DEFINE_string(dwarf_section_vmbase, "", "vm base addr for dwarf sections, in hex format");
DEFINE_bool(arm64, false, "build 64 bit symbol file (ARM64)");

namespace {
    void init_gflag_config(int& argc, char**& argv) {
        std::string usage("recreate the symbol file from ida output for ios application, e.g.\n");
        usage += std::string("\n") + argv[0] + " --uuid \"14494083-a184-31e2-946b-3f942a402952\" --raw_ida_symbol \"/tmp/symbols.txt\" --dwarf_section_vmbase 0x40000 --output \"/path/to/save/loadable_symbol\" \n";
        usage += std::string("then if no error occurs, a fresh symbol file will be created at /path/to/save/loadable_symbol");
        ::google::SetUsageMessage(usage);
        
        ::google::ParseCommandLineFlags(&argc, &argv, true);
        
        if (FLAGS_uuid.length() == 0 ||
            FLAGS_raw_ida_symbol.length() == 0 ||
            FLAGS_dwarf_section_vmbase.length() == 0 ||
            FLAGS_output.length() == 0) {
            ::google::ShowUsageWithFlags(argv[0]);
            throw Exception(ExceptionCode::kParamInvalid, "invalid param, please check the usage");
        }
    }
    
    std::string build_failed_result(const Exception& exception) {
        return util::build_string([&](std::ostringstream& ss) {
            ss << "{\"error\":" << (int)exception.code() << ", \"description\":\"" << exception.description() << "\"}";
        });
    }
    
    template <typename T>
    std::vector<Symbol<T>> read_raw_symbol_data_from_path(const std::string& path) {
        NSError* error = nil;
        NSString* content = [NSString stringWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()] encoding:NSUTF8StringEncoding error:&error];
        assert(error == nil);
        NSArray* lines = [content componentsSeparatedByString:@"\n"];
        std::vector<Symbol<T>> symbols;
        for (NSString* line in lines) {
            if (line.length > 0) {
                NSArray* parts = [line componentsSeparatedByString:@"\t"];
                assert(parts.count == 3);
                std::string name([parts[2] UTF8String]);
                T base = (T)std::stoull(std::string([parts[0] UTF8String]), nullptr, 16);
                T end = (T)std::stoull(std::string([parts[1] UTF8String]), nullptr, 16);;
                symbols.push_back(Symbol<T>(name, base, end));
            }
        }
        return symbols;
    }
    
    template <typename T>
    void dump_symbol_to_file(const std::string& uuid,
                             const std::string& symbol_file_path,
                             T vmbase,
                             const std::string& output_file_path) {
        Macho<T> macho;
        auto symbols = read_raw_symbol_data_from_path<T>(symbol_file_path);
        auto buffer = macho.dump(uuid, symbols, vmbase);
        util::write<T>(output_file_path, 0, (T)buffer.size(), &buffer[0]);
        std::cout << "create symbol file success: " << output_file_path << std::endl;
    }
}

int main(int argc, char* argv[]) {
    
    try {
        init_gflag_config(argc, argv);
        
        if (FLAGS_arm64) {
            uint64_t vmbase = std::stoull(FLAGS_dwarf_section_vmbase, nullptr, 16);
            dump_symbol_to_file<uint64_t>(FLAGS_uuid, FLAGS_raw_ida_symbol, vmbase, FLAGS_output);
        } else {
            uint32_t vmbase = (uint32_t)std::stoul(FLAGS_dwarf_section_vmbase, nullptr, 16);
            dump_symbol_to_file<uint32_t>(FLAGS_uuid, FLAGS_raw_ida_symbol, vmbase, FLAGS_output);
        }
        return 0;
    } catch (Exception& e) {
        std::cerr << build_failed_result(e) << std::endl;
        return (int)e.code();
    }
}
