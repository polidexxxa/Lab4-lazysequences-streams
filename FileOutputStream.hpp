#ifndef _FILEOUTPUTSTREAM_HPP_
#define _FILEOUTPUTSTREAM_HPP_

#include "OutputStream.hpp"
#include "Serializer.hpp"
#include <fstream>

template <typename T>
class FileOutputStream : public OutputStream<T> {
private:
    std::string filename;
    const ISerializer<T>* serializer;
    std::ofstream file;
    bool isOpen;
    size_t currentPosition;

    void EnsureOpen() const {
        if (!isOpen) throw UnsupportedOperationException("Stream is not open");
    }

public:
    FileOutputStream(const std::string& name, const ISerializer<T>* ser)
        : filename(name), serializer(ser), isOpen(false), currentPosition(0) {
        if (ser == nullptr) throw InvalidArgumentException("Serializer cannot be null");
    }

    ~FileOutputStream() override {
        if (isOpen) Close();
    }

    void Open() override {
        if (isOpen) return;
        file.open(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filename);
        }
        isOpen = true;
        currentPosition = 0;
    }

    void Close() override {
        if (!isOpen) return;
        file.close();
        isOpen = false;
    }

    size_t GetPosition() const override { 
        return currentPosition; 
    }

    size_t Write(const T& item) override {
        EnsureOpen();
        std::string data = serializer->Serialize(item);
        file << data << "\n";
        currentPosition++;
        return currentPosition;
    }
};

#endif // _FILEOUTPUTSTREAM_HPP_