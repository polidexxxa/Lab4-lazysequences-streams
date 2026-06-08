#ifndef _REMOVENODE_HPP_
#define _REMOVENODE_HPP_

#include "LazyNode.hpp"
#include "Ordinal.hpp"
#include "Exceptions.hpp"

template <typename T>
class RemoveNode : public LazyNode<T> {
private:
    LazyNode<T>* baseNode;
    Ordinal removePosition;
    Ordinal removeLength;

public:
    RemoveNode(LazyNode<T>* base, const Ordinal& position, const Ordinal& length = Ordinal(1))
        : baseNode(base), removePosition(position), removeLength(length) {
        if (baseNode == nullptr) throw InvalidArgumentException("RemoveNode base cannot be null");
    }

    RemoveNode(const RemoveNode<T>& other)
        : baseNode(other.baseNode->Clone()),
          removePosition(other.removePosition),
          removeLength(other.removeLength) {}

    ~RemoveNode() override { delete baseNode; }

    T Get(const Ordinal& index) override {
        if (index < removePosition) {
            return baseNode->Get(index);
        }
        
        Ordinal shiftedIndex = index + removeLength;
        return baseNode->Get(shiftedIndex);
    }

    Ordinal GetLength() const override {
        Ordinal baseLen = baseNode->GetLength();
        
        if (baseLen.IsInfinite()) return baseLen; 
        
        int newLen = baseLen.GetFinitePart() - removeLength.GetFinitePart();
        return Ordinal(newLen < 0 ? 0 : newLen);
    }

    LazyNode<T>* Clone() const override { 
        return new RemoveNode<T>(*this); 
    }
};

#endif // _REMOVENODE_HPP_