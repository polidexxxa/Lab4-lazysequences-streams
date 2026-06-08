#ifndef _LAZYNODE_HPP_
#define _LAZYNODE_HPP_

#include "Ordinal.hpp"

template <typename T>
class LazyNode {
public:
    virtual ~LazyNode() = default;

    virtual T Get(const Ordinal& index) = 0;
    virtual Ordinal GetLength() const = 0;
    virtual LazyNode<T>* Clone() const = 0;
};

#endif // _LAZYNODE_HPP_