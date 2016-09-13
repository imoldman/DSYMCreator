//
//  exception.h
//  insert_dylib_to_macho
//
//  Created by oldman on 6/13/15.
//  Copyright (c) 2015 imoldman. All rights reserved.
//

#ifndef __insert_dylib_to_macho__exception__
#define __insert_dylib_to_macho__exception__

#include <exception>
#include <string>

enum class ExceptionCode
{
    kShouldNotOccur = 1,
    kParamInvalid,
    kReadFailed,
    kWriteFailed,
    kNotImplement,
};

class Exception : public std::exception {
public:
    Exception(ExceptionCode code, const std::string& description);
    ExceptionCode code() const;
    const std::string& description() const;
    virtual const char* what() const noexcept;
private:
    ExceptionCode code_;
    std::string description_;
    std::string what_;
};

#endif /* defined(__insert_dylib_to_macho__exception__) */
