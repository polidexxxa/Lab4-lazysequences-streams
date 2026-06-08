#ifndef _STREAM_HPP_
#define _STREAM_HPP_

#include "Exceptions.hpp"

class Stream {
public:
    virtual ~Stream() = default;

    virtual void Open() = 0;
    virtual void Close() = 0;
    
    virtual size_t GetPosition() const = 0;
};

#endif // _STREAM_HPP_