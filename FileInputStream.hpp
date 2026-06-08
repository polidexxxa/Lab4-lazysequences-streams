#ifndef _FILEINPUTSTREAM_HPP_
#define _FILEINPUTSTREAM_HPP_

#include "InputStream.hpp"
#include "Serializer.hpp"
#include <fstream>

template <typename T>
class FileInputStream : public InputStream<T> {
private:
    std::string filename;
    const IDeserializer<T>* deserializer;
    std::ifstream file;
    bool isOpen;
    size_t currentPosition;
    

    std::string nextLine;
    bool hasNext;

    void EnsureOpen() const {
        if (!isOpen) throw UnsupportedOperationException("Stream is not open");
    }

    void ReadAhead() {
        if (std::getline(file, nextLine)) {
            hasNext = true;
        } else {
            hasNext = false;
        }
    }

public:
    FileInputStream(const std::string& name, const IDeserializer<T>* des)
        : filename(name), deserializer(des), isOpen(false), currentPosition(0), hasNext(false) {
        if (des == nullptr) throw InvalidArgumentException("Deserializer cannot be null");
    }

    ~FileInputStream() override {
        if (isOpen) Close();
    }

    void Open() override {
        if (isOpen) return;
        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        isOpen = true;
        currentPosition = 0;
        ReadAhead();
    }

    void Close() override {
        if (!isOpen) return;
        file.close();
        isOpen = false;
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
        if (!hasNext) throw EndOfStreamException();

        T value = deserializer->Deserialize(nextLine);
        ReadAhead();
        currentPosition++;
        return value;
    }

    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t index) override {
        EnsureOpen();
        file.clear();
        file.seekg(0, std::ios::beg);
        currentPosition = 0;
        ReadAhead();

        while (currentPosition < index && hasNext) {
            ReadAhead();
            currentPosition++;
        }
        return currentPosition;
    }

    bool IsCanGoBack() const override { 
        return true; 
    }
};

#endif // _FILEINPUTSTREAM_HPP_