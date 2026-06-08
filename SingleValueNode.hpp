#ifndef _SINGLEVALUENODE_HPP_
#define _SINGLEVALUENODE_HPP_

#include "LazyNode.hpp"
#include "Ordinal.hpp"
#include "Exceptions.hpp"

template <typename T>
class SingleValueNode : public LazyNode<T> {
private:
    T value;

public:
    explicit SingleValueNode(const T& val) : value(val) {}

    T Get(const Ordinal& index) override {
        if (index == Ordinal(0)) {
            return value;
        }
        throw IndexOutOfRangeException("SingleValueNode has only index 0");
    }

    Ordinal GetLength() const override {
        return Ordinal(1);
    }

    LazyNode<T>* Clone() const override {
        return new SingleValueNode<T>(value);
    }
};

#endif // SINGLEVALUENODE_HPP