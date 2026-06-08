#ifndef _SOURCENODE_HPP_
#define _SOURCENODE_HPP_

#include "LazyNode.hpp"
#include "FixedSizeQueue.hpp"
#include "Generator.hpp"
#include "Exceptions.hpp"

template <typename T>
class SourceNode : public LazyNode<T> {
private:
    mutable FixedSizeQueue<T> materialized;
    mutable int totalGenerated;
    Generator<T>* generator;
    Ordinal length;

public:
    SourceNode(Generator<T>* gen, const Ordinal& nodeLength, Sequence<T>* initial = nullptr, int windowSize = 100)
        : materialized(windowSize), totalGenerated(0), generator(gen), length(nodeLength) {
        
        if (gen == nullptr) throw InvalidArgumentException("Generator cannot be null");
        
        if (initial != nullptr) {
            IEnumerator<T>* enumerator = initial->GetEnumerator();
            while (enumerator->MoveNext()) {
                materialized.Enqueue(enumerator->GetCurrent());
                totalGenerated++;
            }
            delete enumerator;
        }
    }


    SourceNode(const SourceNode<T>& other)
        : materialized(other.materialized.GetMaxSize()),
          totalGenerated(other.totalGenerated),
          generator(other.generator->Clone()),
          length(other.length) {
          
        auto items = other.materialized.GetItems();
        for (int i = 0; i < items.GetLength(); i++) {
            materialized.Enqueue(items.Get(i));
        }
    }

    ~SourceNode() override { delete generator; }

    T Get(const Ordinal& index) override {
        if (!index.IsFinite()) {
            throw UnsupportedOperationException("SourceNode cannot process transfinite indices directly");
        }

        int i = index.GetFinitePart();
        if (i < 0) throw IndexOutOfRangeException("Negative index");
        if (length.IsFinite() && index >= length) throw IndexOutOfRangeException("Index out of range");

        while (totalGenerated <= i) {
            if (!generator->HasNext()) throw IndexOutOfRangeException("Index out of range");
            materialized.Enqueue(generator->GetNext());
            totalGenerated++;
        }

        int startIndex = totalGenerated - materialized.GetLength();
        
        if (i < startIndex) {
            throw IndexOutOfRangeException("Element at index " + std::to_string(i) + " is no longer in the cache window (evicted).");
        }

        return materialized.Get(i - startIndex);
    }

    Ordinal GetLength() const override { 
        return length; 
    }

    LazyNode<T>* Clone() const override { 
        return new SourceNode<T>(*this); 
    }
};

#endif // _SOURCENODE_HPP_