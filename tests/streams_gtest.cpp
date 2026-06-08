#include <gtest/gtest.h>
#include <fstream>
#include "../FileInputStream.hpp"
#include "../FileOutputStream.hpp"
#include "../StandartSerializers.hpp"
#include "../SequenceInputStream.hpp"
#include "../SequenceOutputStream.hpp"
#include "../ArraySequence.hpp"
#include "../Exceptions.hpp"

//FILE STREAMS
class FileStreamTest : public ::testing::Test {
protected:
    std::string testFile = "gtest_stream.txt";
    IntSerializer intSer;
    IntDeserializer intDeser;
    StringSerializer strSer;
    StringDeserializer strDeser;

    void TearDown() override {
        std::remove(testFile.c_str());
    }
};

//Int
TEST_F(FileStreamTest, WriteAndReadIntegers) {
    FileOutputStream<int> out(testFile, &intSer);
    out.Open();
    out.Write(10);
    out.Write(-50);
    out.Close();

    FileInputStream<int> in(testFile, &intDeser);
    in.Open();
    EXPECT_EQ(in.Read(), 10);
    EXPECT_EQ(in.Read(), -50);
    EXPECT_TRUE(in.IsEndOfStream());
    in.Close();
}

//String
TEST_F(FileStreamTest, WriteAndReadStrings) {
    FileOutputStream<std::string> out(testFile, &strSer);
    out.Open();
    out.Write("Hello");
    out.Write("World");
    out.Close();

    FileInputStream<std::string> in(testFile, &strDeser);
    in.Open();
    EXPECT_EQ(in.Read(), "Hello");
    EXPECT_EQ(in.Read(), "World");
    in.Close();
}

//исключения
TEST_F(FileStreamTest, ReadPastEndOfStreamThrows) {
    std::ofstream touch(testFile); touch.close(); // пустой файл

    FileInputStream<int> in(testFile, &intDeser);
    in.Open();
    
    EXPECT_TRUE(in.IsEndOfStream());
    EXPECT_THROW(in.Read(), std::exception);
    in.Close();
}

//десериализация неверных данных
TEST_F(FileStreamTest, ReadInvalidDataThrows) {
    std::ofstream badFile(testFile);
    badFile << "NOT_A_NUMBER";
    badFile.close();

    FileInputStream<int> in(testFile, &intDeser);
    in.Open();
    
    EXPECT_THROW(in.Read(), std::runtime_error);
    in.Close();
}

// Запись и чтение данных
TEST_F(FileStreamTest, LargeVolumeData) {
    const int DATA_SIZE = 50000; 
    
    FileOutputStream<int> out(testFile, &intSer);
    out.Open();
    for (int i = 0; i < DATA_SIZE; ++i) {
        out.Write(i);
    }
    out.Close();

    FileInputStream<int> in(testFile, &intDeser);
    in.Open();
    for (int i = 0; i < DATA_SIZE; ++i) {
        ASSERT_FALSE(in.IsEndOfStream()); 
        EXPECT_EQ(in.Read(), i);
    }
    
    EXPECT_TRUE(in.IsEndOfStream());
    in.Close();
}

// Проверка сброса позиции чтения
TEST_F(FileStreamTest, ResetReadPositionByReopening) {
    FileOutputStream<int> out(testFile, &intSer);
    out.Open();
    out.Write(100);
    out.Write(200);
    out.Close();

    FileInputStream<int> in(testFile, &intDeser);
    in.Open();
    EXPECT_EQ(in.Read(), 100);
    
    in.Close();
    in.Open();
    
    EXPECT_EQ(in.Read(), 100);
    EXPECT_EQ(in.Read(), 200);
    EXPECT_TRUE(in.IsEndOfStream());
    in.Close();
}

//SEQUENCE STREAMS
class SequenceStreamTest : public ::testing::Test {
protected:
    ArraySequence<int>* seq;

    void SetUp() override {
        seq = new ArraySequence<int>();
    }

    void TearDown() override {
        delete seq;
    }
};

//Тест записи в SequenceOutputStream
TEST_F(SequenceStreamTest, WriteToSequence) {
    SequenceOutputStream<int> outStream(seq);
    
    EXPECT_THROW(outStream.Write(5), UnsupportedOperationException);
    outStream.Open();
    EXPECT_EQ(outStream.GetPosition(), 0);
    
    outStream.Write(10);
    outStream.Write(20);
    outStream.Write(30);
    
    EXPECT_EQ(outStream.GetPosition(), 3);
    EXPECT_EQ(seq->GetLength(), 3);
    EXPECT_EQ(seq->Get(0), 10);
    EXPECT_EQ(seq->Get(2), 30);
    
    outStream.Close();
    EXPECT_THROW(outStream.Write(40), UnsupportedOperationException);
}

//Тест чтения из SequenceInputStream
TEST_F(SequenceStreamTest, ReadFromSequence) {
    seq->Append(100);
    seq->Append(200);
    seq->Append(300);

    SequenceInputStream<int> inStream(seq);
    
    EXPECT_THROW(inStream.Read(), UnsupportedOperationException); 

    inStream.Open();
    EXPECT_EQ(inStream.GetPosition(), 0);
    EXPECT_FALSE(inStream.IsEndOfStream());
    
    EXPECT_EQ(inStream.Read(), 100);
    EXPECT_EQ(inStream.GetPosition(), 1);
    
    EXPECT_EQ(inStream.Read(), 200);
    EXPECT_EQ(inStream.Read(), 300);
    
    EXPECT_TRUE(inStream.IsEndOfStream());
    EXPECT_THROW(inStream.Read(), EndOfStreamException); 
    
    inStream.Close();
}

//Запись + чтение
TEST_F(SequenceStreamTest, FullCycleWriteAndRead) {
    //Запись
    SequenceOutputStream<int> outStream(seq);
    outStream.Open();
    outStream.Write(77);
    outStream.Write(88);
    outStream.Write(99);
    outStream.Close();
    
    EXPECT_EQ(seq->GetLength(), 3);
    
    //Чтение
    SequenceInputStream<int> inStream(seq);
    inStream.Open();
    EXPECT_EQ(inStream.Read(), 77);
    EXPECT_EQ(inStream.Read(), 88);
    EXPECT_EQ(inStream.Read(), 99);
    
    EXPECT_TRUE(inStream.IsEndOfStream());
    inStream.Close();
}

//Sequence Streams с std::string
TEST_F(SequenceStreamTest, WriteAndReadStrings) {
    ArraySequence<std::string> strSeq;

    //Пишем
    SequenceOutputStream<std::string> out(&strSeq);
    out.Open();
    out.Write("Hello");
    out.Write("Polymorphism");
    out.Close();
    
    EXPECT_EQ(strSeq.GetLength(), 2);
    
    //Читаем
    SequenceInputStream<std::string> in(&strSeq);
    in.Open();
    EXPECT_EQ(in.Read(), "Hello");
    EXPECT_EQ(in.Read(), "Polymorphism");
    EXPECT_TRUE(in.IsEndOfStream());
    in.Close();
}

//Тест Seek
TEST_F(SequenceStreamTest, SeekNavigation) {
    for (int i = 0; i < 5; i++) seq->Append(i * 10); // 0, 10, 20, 30, 40

    SequenceInputStream<int> inStream(seq);
    inStream.Open();
    
    EXPECT_TRUE(inStream.IsCanSeek());
    EXPECT_TRUE(inStream.IsCanGoBack());

    EXPECT_EQ(inStream.Read(), 0); 
    
    inStream.Seek(3);
    EXPECT_EQ(inStream.GetPosition(), 3);
    EXPECT_EQ(inStream.Read(), 30);
    
    inStream.Seek(1);
    EXPECT_EQ(inStream.Read(), 10);

    EXPECT_THROW(inStream.Seek(100), IndexOutOfRangeException);
    
    inStream.Close();
}

//Исключения при инициализации
TEST_F(SequenceStreamTest, NullPointerInitialization) {
    EXPECT_THROW(SequenceOutputStream<int> out(nullptr), InvalidArgumentException);
    EXPECT_THROW(SequenceInputStream<int> in(nullptr), InvalidArgumentException);
}