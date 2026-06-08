#ifndef _SEQUENCEINPUTSTREAM_HPP_
#define _SEQUENCEINPUTSTREAM_HPP_

#include "InputStream.hpp"
#include "Sequence.hpp"
#include "LazySequence.hpp"

template <typename T>
class SequenceInputStream : public InputStream<T> {
private:
    const Sequence<T>* sequence;
    IEnumerator<T>* enumerator;
    bool isOpen;
    size_t currentPosition;
    bool hasNext; 

    void EnsureOpen() const {
        if (!isOpen) throw UnsupportedOperationException("Stream is not open");
    }

public:
    explicit SequenceInputStream(const Sequence<T>* seq)
        : sequence(seq), enumerator(nullptr), isOpen(false), currentPosition(0), hasNext(false) {
        if (seq == nullptr) throw InvalidArgumentException("Sequence cannot be null");
    }

    ~SequenceInputStream() override {
        if (isOpen) Close();
    }

    void Open() override {
        if (isOpen) return;
        enumerator = sequence->GetEnumerator();
        isOpen = true;
        currentPosition = 0;
        hasNext = enumerator->MoveNext();
    }

    void Close() override {
        if (!isOpen) return;
        delete enumerator;
        enumerator = nullptr;
        isOpen = false;
        currentPosition = 0;
        hasNext = false;
    }

    size_t GetPosition() const override {
        return currentPosition;
    }

    bool IsEndOfStream() const override {
        EnsureOpen();
        return !hasNext;
    }

    T Read() override {
        EnsureOpen();
        if (!hasNext) {
            throw EndOfStreamException();
        }

        T value = enumerator->GetCurrent();
        hasNext = enumerator->MoveNext();
        currentPosition++;
        
        return value;
    }

    bool IsCanSeek() const override {
        return true; 
    }

    size_t Seek(size_t index) override {
        EnsureOpen();
        
        int len = sequence->GetLength();
        if (len != -1 && index >= static_cast<size_t>(len)) {
            throw IndexOutOfRangeException("Seek position out of range");
        }

        delete enumerator;
        enumerator = sequence->GetEnumerator();
        
        currentPosition = 0;
        hasNext = enumerator->MoveNext();
        
        while (currentPosition < index && hasNext) {
            hasNext = enumerator->MoveNext();
            currentPosition++;
        }

        return currentPosition;
    }

    bool IsCanGoBack() const override {
        return true;
    }
};

#endif // _SEQUENCEINPUTSTREAM_HPP_