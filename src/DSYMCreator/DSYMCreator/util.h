//
//  Created by oldman on 6/9/15.
//

#ifndef __insert_dylib_to_macho__util__
#define __insert_dylib_to_macho__util__

#include <string>
#include <functional>
#include <sstream>

#include "common.h"
#include "exception.h"

namespace util {    
    std::string build_string(std::function<void (std::ostringstream&)> f);
    
    template <typename T>
    void read(FILE* file, T from, T size, void* buffer);
    
    template <typename T>
    void write(FILE* file, T from, T size, void* buffer);
    
    template <typename T>
    void read(const std::string& file, T from, T size, void* buffer);
    
    template <typename T>
    void write(const std::string& file, T from, T size, void* buffer);
    
    template <typename T>
    void append_to_buffer(ByteBuffer& buffer, const T& t);
    template <>
    void append_to_buffer(ByteBuffer& buffer, const ByteBuffer& t);


    // implementation
    template <typename T>
    void read(FILE* file, T from, T size, void* buffer) {
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
    
    template <typename T>
    void write(FILE* file, T from, T size, void* buffer) {
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
    
    template <typename T>
    void read(const std::string& file, T from, T size, void* buffer) {
        FILE* handler = fopen(file.c_str(), "rb");
        if (handler == NULL) {
            throw Exception(ExceptionCode::kShouldNotOccur, util::build_string([&](std::ostringstream& ss) {
                ss << "open file failed, file: " << file << ", errno: " << errno;
            }));
        }
        return read(handler, from, size, buffer);
    }
    
    template <typename T>
    void write(const std::string& file, T from, T size, void* buffer) {
        FILE* handler = fopen(file.c_str(), "wb");
        if (handler == NULL) {
            throw Exception(ExceptionCode::kShouldNotOccur, util::build_string([&](std::ostringstream& ss) {
                ss << "write file failed, file: " << file << ", errno: " << errno;
            }));
        }
        return write(handler, from, size, buffer);
    }
    
    template <typename T>
    void append_to_buffer(ByteBuffer& buffer, const T& t) {
        buffer.insert(buffer.end(), (uint8_t*)&t, (uint8_t*)(&t+1));
    }    
}
#endif /* defined(__insert_dylib_to_macho__util__) */
