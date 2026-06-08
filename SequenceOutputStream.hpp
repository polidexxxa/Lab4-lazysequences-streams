#ifndef _SEQUENCEOUTPUTSTREAM_HPP_
#define _SEQUENCEOUTPUTSTREAM_HPP_

#include "OutputStream.hpp"
#include "Sequence.hpp"

template <typename T>
class SequenceOutputStream : public OutputStream<T> {
private:
    Sequence<T>* sequence;
    bool isOpen;
    size_t currentPosition;

    void EnsureOpen() const {
        if (!isOpen) throw UnsupportedOperationException("Stream is not open");
    }

public:
    explicit SequenceOutputStream(Sequence<T>* seq)
        : sequence(seq), isOpen(false), currentPosition(0) {
        if (seq == nullptr) throw InvalidArgumentException("Sequence cannot be null");
    }

    ~SequenceOutputStream() override {
        if (isOpen) Close();
    }

    void Open() override {
        if (isOpen) return;
        isOpen = true;
        currentPosition = sequence->GetLength();
    }

    void Close() override {
        isOpen = false;
    }

    size_t GetPosition() const override {
        return currentPosition;
    }

    size_t Write(const T& item) override {
        EnsureOpen();
        sequence->Append(item);
        currentPosition++;
        return currentPosition;
    }
};

#endif // _SEQUENCEOUTPUTSTREAM_HPP_