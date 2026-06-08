#ifndef _ARRAYSEQUENCE_HPP_
#define _ARRAYSEQUENCE_HPP_

#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include "Exceptions.hpp"
#include <functional>

//Mutable
template<typename T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;

protected:
    Sequence<T>* CreateEmptySameType() const override {
        return new ArraySequence<T>();
    }

    void AppendInternal(const T& item) override {
        int oldSize = items->GetSize();
        items->Resize(oldSize + 1);
        items->Set(oldSize, item);
    }

    void PrependInternal(const T& item) override {
        int oldSize = items->GetSize();
        items->Resize(oldSize + 1);

        for (int i = oldSize; i > 0; i--) {
            items->Set(i, items->Get(i - 1));
        }
        items->Set(0, item);
    }

    void InsertAtInternal(const T& item, int index) override {
        if (index < 0 || index > items->GetSize()) {
            throw IndexOutOfRangeException("Index out of range for insertion");
        }

        int oldSize = items->GetSize();
        items->Resize(oldSize + 1);
        
        for (int i = oldSize; i > index; i--) {
            items->Set(i, items->Get(i - 1));
        }
        items->Set(index, item);
    }

    void SetInternal(int index, const T& item) override {
        items->Set(index, item);
    }
    
public:
    ArraySequence(T* items, int count) {
        this->items = new DynamicArray<T>(items, count);
    }

    ArraySequence() {
        this->items = new DynamicArray<T>(0);
    }

    explicit ArraySequence(const DynamicArray<T>& array) {
        this->items = new DynamicArray<T>(array);
    }

    ArraySequence(const ArraySequence<T>& other) {
        this->items = new DynamicArray<T>(*(other.items));
    }

    ~ArraySequence() override {
        delete items;
    }

    Sequence<T>* Instance() override { 
        return this;
    }

    ArraySequence<T>& operator=(const ArraySequence<T>& other) {
        if (this != &other) {
            delete items;
            items = new DynamicArray<T>(*(other.items));
        }
        return *this;
    }

    T GetFirst() const override {
        if (items->GetSize() == 0) {
            throw EmptyContainerException("Cannot get first element from empty sequence");
        }
        return items->Get(0);
    }

    T GetLast() const override {
        if (items->GetSize() == 0) {
            throw EmptyContainerException("Cannot get last element from empty sequence");
        }
        return items->Get(items->GetSize() - 1);
    }

    T Get(int index) const override {
        return items->Get(index);
    }

    int GetLength() const override {
        return items->GetSize();
    }

    const DynamicArray<T>& GetInternalArray() const {
        return *items;
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(std::function<ResultType(const T&)> func) const {
        ResultType* temp = new ResultType[this->GetLength()];
        
        IEnumerator<T>* enumerator = this->GetEnumerator();
        for (int i = 0; enumerator->MoveNext(); i++) {
            temp[i] = func(enumerator->GetCurrent());
        }
        delete enumerator;

        auto* result = new ArraySequence<ResultType>(temp, this->GetLength());
        delete[] temp;
        return result;
    }

    Sequence<T>* Clone() const override {
        return new ArraySequence<T>(*items);
    }

    T& operator[](int index) override {
        return items->GetRef(index);
    }
    
    const T& operator[](int index) const override {
        return items->GetRef(index);
    }
    
    IEnumerator<T>* GetEnumerator() const override {
        return new ArrayEnumerator<T>(items);
    }


};

//=============================================================================
//                              IMMUTABLE
//=============================================================================

template<typename T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    Sequence<T>* CreateEmptySameType() const override {
        return new ImmutableArraySequence<T>();
    }

public:
    using ArraySequence<T>::ArraySequence;

    ImmutableArraySequence() : ArraySequence<T>() {}

    ImmutableArraySequence(const ImmutableArraySequence<T>& other) 
        : ArraySequence<T>(other) {}
    
    Sequence<T>* Instance() override { 
        return this->Clone(); 
    }
    

    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    T& operator[](int index) override {
        throw UnsupportedOperationException("Immutable sequence does not support mutable access");
    }
    
    using ArraySequence<T>::operator[];
  
};




#endif //_ARRAYSEQUENCE_HPP_
