#ifndef _FIXEDSIZEQUEUE_HPP_
#define _FIXEDSIZEQUEUE_HPP_

#include "LinkedList.hpp"
#include "Exceptions.hpp"

template<typename T>
class FixedSizeQueue {
private:
    LinkedList<T> items;
    int maxSize;
    
public:
    FixedSizeQueue(int size) : maxSize(size) {
        if (size < 0) {
            throw InvalidArgumentException("Queue size cannot be negative");
        }
    }
    
    void Enqueue(const T& item) {
        items.Append(item);

        while (items.GetLength() > maxSize) {
            items.RemoveFirst();
        }
    }
    
    T Get(int index) const {
        if (index < 0 || index >= items.GetLength()) {
            throw IndexOutOfRangeException("Index out of range in queue");
        }
        return items.Get(index);
    }
    
    int GetLength() const {
        return items.GetLength();
    }
    
    int GetMaxSize() const {
        return maxSize;
    }
    
    bool IsEmpty() const {
        return items.GetLength() == 0;
    }
    
    void Clear() {
        items = LinkedList<T>();
    }
    
    const LinkedList<T>& GetItems() const {
        return items;
    }
};

#endif // _FIXEDSIZEQUEUE_HPP_