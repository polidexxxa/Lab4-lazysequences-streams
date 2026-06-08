#ifndef _STANDARDSERIALIZERS_HPP_
#define _STANDARDSERIALIZERS_HPP_

#include "Serializer.hpp"
#include <string>
#include <stdexcept>


class IntSerializer : public ISerializer<int> {
public:
    std::string Serialize(const int& item) const override {
        return std::to_string(item); 
    }
};

class IntDeserializer : public IDeserializer<int> {
public:
    int Deserialize(const std::string& data) const override {
        try {
            return std::stoi(data); 
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Deserialization error: '" + data + "' is not a valid integer");
        } catch (const std::out_of_range& e) {
            throw std::runtime_error("Deserialization error: '" + data + "' is out of int range");
        }
    }
};


class StringSerializer : public ISerializer<std::string> {
public:
    std::string Serialize(const std::string& item) const override {
        return item; 
    }
};

class StringDeserializer : public IDeserializer<std::string> {
public:
    std::string Deserialize(const std::string& data) const override {
        return data; 
    }
};

#endif // _STANDARDSERIALIZERS_HPP_