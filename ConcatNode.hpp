#ifndef _CONCATNODE_HPP_
#define _CONCATNODE_HPP_

#include "LazyNode.hpp"
#include "Ordinal.hpp"

template <typename T>
class ConcatNode : public LazyNode<T> {
private:
    LazyNode<T>* left;
    LazyNode<T>* right;

public:
    ConcatNode(LazyNode<T>* leftNode, LazyNode<T>* rightNode)
        : left(leftNode), right(rightNode) {}

    ConcatNode(const ConcatNode<T>& other)
        : left(other.left->Clone()), right(other.right->Clone()) {}

    ~ConcatNode() override {
        delete left;
        delete right;
    }

    T Get(const Ordinal& index) override {
        Ordinal leftLength = left->GetLength();

        if (index < leftLength) {
            return left->Get(index);
        }

        Ordinal shifted = Ordinal::SubtractLeft(leftLength, index);
        return right->Get(shifted);
    }

    Ordinal GetLength() const override {
        return left->GetLength() + right->GetLength();
    }

    LazyNode<T>* Clone() const override {
        return new ConcatNode<T>(*this);
    }
};

#endif // _CONCATNODE_HPP_