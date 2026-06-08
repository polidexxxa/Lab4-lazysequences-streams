#ifndef _SERIALIZER_HPP_
#define _SERIALIZER_HPP_

#include <string>

//String -> T
template <typename T>
class IDeserializer {
public:
    virtual ~IDeserializer() = default;
    virtual T Deserialize(const std::string& data) const = 0;
};

//T -> String
template <typename T>
class ISerializer {
public:
    virtual ~ISerializer() = default;
    virtual std::string Serialize(const T& item) const = 0;
};

#endif // _SERIALIZER_HPP_