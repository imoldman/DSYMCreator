//
//  exception.cpp
//  insert_dylib_to_macho
//
//  Created by oldman on 6/13/15.
//  Copyright (c) 2015 imoldman. All rights reserved.
//

#include "exception.h"
#include <sstream>

Exception::Exception(ExceptionCode code, const std::string& description) : code_(code), description_(description) {
    std::ostringstream ss(what_);
    ss << "code: " << (int)code << ", detail: " << description;
}

const char* Exception::what() const noexcept {
    return what_.c_str();
}

ExceptionCode Exception::code() const {
    return code_;
}

const std::string& Exception::description() const {
    return description_;
}