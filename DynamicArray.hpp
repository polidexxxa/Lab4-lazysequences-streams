#ifndef _DYNAMICARRAY_HPP_
#define _DYNAMICARRAY_HPP_

#include "Exceptions.hpp"
#include "IEnumerator.hpp"

template<typename T>
class DynamicArray {
private:
    T* data;
    size_t size;
    size_t capacity;

public:

    DynamicArray(T* items, int count) {
        if (count < 0) {
            throw InvalidArgumentException("Size cannot be negative");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Items array cannot be null for non-zero size");
        }
        
        size = count;
        capacity = count;
        data = new T[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = items[i];
        }
    }

    explicit DynamicArray(int size) {
        if (size < 0) {
            throw InvalidArgumentException("Size cannot be negative");
        }

        this->size = size;
        this->capacity = size;
        data = new T[capacity];
    }

    DynamicArray(const DynamicArray<T>& other) {
        size = other.size;
        capacity = other.capacity;
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];
            for (size_t i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    DynamicArray(DynamicArray<T>&& other) noexcept 
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    ~DynamicArray() {
        delete[] data;
    }


    T Get(int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        return data[index];
    }

    DynamicArray<T>* GetSubArray(int startIndex, int endIndex) const {
        if (startIndex < 0 || startIndex >= size || 
            endIndex < 0 || endIndex >= size || 
            startIndex > endIndex) {
            throw IndexOutOfRangeException("Invalid subarray indices");
        }

        int newSize = endIndex - startIndex + 1;
        T* tempData = new T[newSize];
        
        for (int i = 0; i < newSize; i++) {
            tempData[i] = data[startIndex + i];
        }
        
        DynamicArray<T>* subArray = new DynamicArray<T>(tempData, newSize);
        delete[] tempData;
        
        return subArray;
    }

    int GetSize() const {
        return size;
    }

    void Set(int index, T value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        data[index] = value;
    }

    void Resize(int newSize) {
        if (newSize < 0) {
            throw InvalidArgumentException("New size cannot be negative");
        }
        if (newSize == size) {
            return;
        }

        if (newSize > capacity) {
            size_t newCapacity = static_cast<size_t>(newSize * 2);
            if (newCapacity < newSize) newCapacity = newSize; 

            T* newData = new T[newCapacity];
            
            for (int i = 0; i < size && i < newSize; i++) {
                newData[i] = data[i];
            }
            
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
        
        size = newSize;
    }


    T& GetRef(int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        return data[index];
    }

    const T& GetRef(int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        return data[index];
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(size) + ")");
        }
        return data[index];
    }


};


//========================================================================

//========================================================================



template<typename T>
class ArrayEnumerator : public IEnumerator<T> {
private:
    DynamicArray<T>* array;
    int currentIndex;
    bool isConst;

public:
    ArrayEnumerator(DynamicArray<T>* arr) : array(arr), currentIndex(-1), isConst(false) {
        if (arr == nullptr) {
            throw InvalidArgumentException("ArrayEnumerator: array cannot be null");
        }
    }
    
    ArrayEnumerator(const DynamicArray<T>* arr) : array(const_cast<DynamicArray<T>*>(arr)), currentIndex(-1), isConst(true) {
        if (arr == nullptr) {
            throw InvalidArgumentException("ArrayEnumerator: array cannot be null");
        }
    }
    
    ArrayEnumerator(const ArrayEnumerator& other) : array(other.array), currentIndex(other.currentIndex), isConst(other.isConst) {}
    
    IEnumerator<T>* Clone() const override {
        return new ArrayEnumerator(*this);
    }
    
    bool MoveNext() override {
        if (array == nullptr) return false; 
        if (currentIndex < array->GetSize() - 1) {
            currentIndex++;
            return true;
        }
        return false;
    }
    
    const T& GetCurrent() const override {
        if (array == nullptr) {
            throw InvalidArgumentException("Enumerator is not attached to an array");
        }
        if (currentIndex < 0 || currentIndex >= array->GetSize()) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return array->GetRef(currentIndex);
    }
    
    T& GetCurrent() override {
        if (array == nullptr) {
            throw InvalidArgumentException("Enumerator is not attached to an array");
        }
        if (isConst) {
            throw UnsupportedOperationException("Cannot modify through const enumerator");
        }
        if (currentIndex < 0 || currentIndex >= array->GetSize()) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return array->GetRef(currentIndex);
    }
    
    void Reset() override {
        currentIndex = -1;
    }
};




#endif //_DYNAMICARRAY_HPP_