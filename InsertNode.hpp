#ifndef _INSERTNODE_HPP_
#define _INSERTNODE_HPP_

#include "LazyNode.hpp"
#include "Ordinal.hpp"

template <typename T>
class InsertNode : public LazyNode<T> {
private:
    LazyNode<T>* base;
    LazyNode<T>* inserted;
    Ordinal position;

public:
    InsertNode(LazyNode<T>* baseNode, LazyNode<T>* insertedNode, const Ordinal& pos)
        : base(baseNode), inserted(insertedNode), position(pos) {}

    InsertNode(const InsertNode<T>& other)
        : base(other.base->Clone()),
          inserted(other.inserted->Clone()),
          position(other.position) {}

    ~InsertNode() override {
        delete base;
        delete inserted;
    }

    T Get(const Ordinal& index) override {
        if (index < position) {
            return base->Get(index);
        }

        Ordinal insertedEnd = position + inserted->GetLength();

        if (index < insertedEnd) {
            Ordinal localIndex = Ordinal::SubtractLeft(position, index);
            return inserted->Get(localIndex);
        }

        Ordinal tailShiftIndex = position + Ordinal::SubtractLeft(insertedEnd, index);
        
        return base->Get(tailShiftIndex);
    }

    Ordinal GetLength() const override {
        return base->GetLength() + inserted->GetLength();
    }

    LazyNode<T>* Clone() const override {
        return new InsertNode<T>(*this);
    }
};

#endif // _INSERTNODE_HPP_