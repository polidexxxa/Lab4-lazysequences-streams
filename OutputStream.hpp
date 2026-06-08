#ifndef _OUTPUTSTREAM_HPP_
#define _OUTPUTSTREAM_HPP_

#include "Stream.hpp"

template <typename T>
class OutputStream : public Stream {
public:
    virtual ~OutputStream() = default;

    virtual size_t Write(const T& item) = 0;
};

#endif // _OUTPUTSTREAM_HPP_