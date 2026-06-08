#ifndef _FIXEDQUEUESEQUENCE_HPP_
#define _FIXEDQUEUESEQUENCE_HPP_

#include "Sequence.hpp"
#include "FixedSizeQueue.hpp"

template <typename T>
class FixedQueueSequence : public Sequence<T> {
private:
    FixedSizeQueue<T> queue;

protected:
    Sequence<T>* CreateEmptySameType() const override { 
        return new FixedQueueSequence<T>(queue.GetMaxSize()); 
    }

    void AppendInternal(const T& item) override { 
        queue.Enqueue(item); 
    }

    void PrependInternal(const T& item) override { throw UnsupportedOperationException("Operation not supported"); }
    void InsertAtInternal(const T& item, int index) override { throw UnsupportedOperationException("Operation not supported"); }
    void SetInternal(int index, const T& item) override { throw UnsupportedOperationException("Operation not supported"); }

public:
    FixedQueueSequence(int capacity) : queue(capacity) {}

    void Enqueue(const T& item) { 
        queue.Enqueue(item); 
    }

    int GetLength() const override {
        return queue.GetLength(); 
    }

    T Get(int index) const override {
        return queue.Get(index); 
    }

    T GetFirst() const override { 
        return queue.Get(0); 
    }

    T GetLast() const override { 
        return queue.Get(queue.GetLength() - 1); 
    }

    Sequence<T>* Clone() const override {
        auto clone = new FixedQueueSequence<T>(queue.GetMaxSize());
        for (int i = 0; i < queue.GetLength(); i++) {
            clone->Enqueue(queue.Get(i));
        }
        return clone;
    }

    Sequence<T>* Instance() override { 
        return Clone(); 
    }

    IEnumerator<T>* GetEnumerator() const override { throw UnsupportedOperationException("Enumerator is not supported"); }
    T& operator[](int index) override { throw UnsupportedOperationException("Mutable access not supported"); }

    const T& operator[](int index) const override { 
        throw UnsupportedOperationException("Operation is not supported");
    }

};

#endif //_FIXEDQUEUESEQUENCE_HPP_