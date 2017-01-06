//
//  Created by oldman on 6/9/15.
//

#include "util.h"

namespace util {
    std::string build_string(std::function<void (std::ostringstream&)> f) {
        std::ostringstream ss;
        f(ss);
        return ss.str();
    }
    
    template <>
    void append_to_buffer<ByteBuffer>(ByteBuffer& buffer, const ByteBuffer& buffer2) {
        buffer.insert(buffer.end(), buffer2.begin(), buffer2.end());
    }
}
