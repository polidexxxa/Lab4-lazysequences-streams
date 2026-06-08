#ifndef _INPUTSTREAM_HPP_
#define _INPUTSTREAM_HPP_

#include "Stream.hpp"

template <typename T>
class InputStream : public Stream {
public:
    virtual ~InputStream() = default;

    virtual T Read() = 0;
    virtual bool IsEndOfStream() const = 0;
    virtual bool IsCanSeek() const = 0;

    virtual size_t Seek(size_t index) = 0;
    virtual bool IsCanGoBack() const = 0;
};

#endif // _INPUTSTREAM_HPP_