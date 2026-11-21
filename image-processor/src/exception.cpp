#include "exception.h"

const char* Exception::what() const noexcept {
    return message_.c_str();
}
