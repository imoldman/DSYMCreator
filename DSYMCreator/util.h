//
//  Created by oldman on 6/9/15.
//

#ifndef __insert_dylib_to_macho__util__
#define __insert_dylib_to_macho__util__

#include <string>
#include <functional>
#include <sstream>

namespace util {
    bool string_start_with(const std::string& str, const std::string& prefix);
    
    std::string build_string(std::function<void (std::ostringstream&)> f);
    
    void read(FILE* file, uint32_t from, uint32_t size, void* buffer);
    
    void write(FILE* file, uint32_t from, uint32_t size, void* buffer);
    
    void read(const std::string& file, uint32_t from, uint32_t size, void* buffer);
    
    void write(const std::string& file, uint32_t from, uint32_t size, void* buffer);
}
#endif /* defined(__insert_dylib_to_macho__util__) */
