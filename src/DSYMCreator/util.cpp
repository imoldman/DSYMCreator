//
//  Created by oldman on 6/9/15.
//

#include "util.h"
#include "exception.h"

namespace util {
    bool string_start_with(const std::string& str, const std::string& prefix) {
        if (str.length() < prefix.length()) {
            return false;
        }
        return str.substr(0, prefix.length()) == prefix;
    }
    
    std::string build_string(std::function<void (std::ostringstream&)> f) {
        std::ostringstream ss;
        f(ss);
        return ss.str();
    }
    
    void read(FILE* file, uint32_t from, uint32_t size, void* buffer) {
        int err = fseek(file, from, SEEK_SET);
        if (err == 0) {
            size_t readed_size = fread(buffer, sizeof(char), size, file);
            if (size == readed_size) {
                return;
            }
        }
        throw Exception(ExceptionCode::kReadFailed, util::build_string([=](std::ostringstream& ss) {
            ss << "read failed, from: " << from << ", size: " << size;
        }));
    }
    
    void write(FILE* file, uint32_t from, uint32_t size, void* buffer) {
        int err = fseek(file, from, SEEK_SET);
        if (err == 0) {
            size_t wroted_size = fwrite(buffer, sizeof(char), size, file);
            if (size == wroted_size) {
                return;
            }
        }
        throw Exception(ExceptionCode::kWriteFailed, util::build_string([=](std::ostringstream& ss) {
            ss << "write failed, from: " << from << ", size: " << size;
        }));
    }
    
    void read(const std::string& file, uint32_t from, uint32_t size, void* buffer) {
        FILE* handler = fopen(file.c_str(), "rb");
        if (handler == NULL) {
            throw Exception(ExceptionCode::kShouldNotOccur, util::build_string([&](std::ostringstream& ss) {
                ss << "open file failed, file: " << file << ", errno: " << errno;
            }));
        }
        return read(handler, from, size, buffer);
    }
    
    void write(const std::string& file, uint32_t from, uint32_t size, void* buffer) {
        FILE* handler = fopen(file.c_str(), "wb");
        if (handler == NULL) {
            throw Exception(ExceptionCode::kShouldNotOccur, util::build_string([&](std::ostringstream& ss) {
                ss << "write file failed, file: " << file << ", errno: " << errno;
            }));
        }
        return write(handler, from, size, buffer);
    }
}
