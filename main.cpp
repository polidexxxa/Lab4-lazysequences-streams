#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include "TuringMachine.hpp"
#include "LazySequence.hpp"
#include "ArraySequence.hpp"
#include "Ordinal.hpp"
#include "FileInputStream.hpp"
#include "FileOutputStream.hpp"
#include "StandartSerializers.hpp"

void PrintMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "                LAB WORK 4               \n";
    std::cout << "=========================================\n";
    std::cout << "1. Turing Machine (Caesar Cipher)\n";
    std::cout << "2. Lazy Sequences\n";
    std::cout << "3. Data Streams (File I/O)\n";
    std::cout << "0. Exit\n";
    std::cout << "=========================================\n";
    std::cout << "Select an option: ";
}

//Для Тьюринга
char ShiftChar(char c, int shift) {
    int s = (shift % 26 + 26) % 26;
    if (c >= 'a' && c <= 'z') return 'a' + (c - 'a' + s) % 26;
    if (c >= 'A' && c <= 'Z') return 'A' + (c - 'A' + s) % 26;
    return c;
}

TuringMachine CreateCaesarMachine(int shift) {
    TuringMachine tm("encrypt", "halt", '_');
    for (int i = 32; i <= 126; i++) {
        char c = static_cast<char>(i);
        if (c == '_') continue;
        char shifted = ShiftChar(c, shift);
        tm.AddRule(TuringRule("encrypt", c, shifted, MoveDirection::Right, "encrypt"));
    }
    tm.AddRule(TuringRule("encrypt", '_', '_', MoveDirection::None, "halt"));
    return tm;
}

std::string ExtractStringFromTape(const TuringTape& tape) {
    std::string result = "";
    for (int i = 0; ; i++) {
        char c = tape.Read(i);
        if (c == '_') break;
        result += c;
    }
    return result;
}

//Машина Тьюринга
void RunTuringMachineUI() {
    std::cout << "\n--- Turing Machine: Caesar Cipher ---\n";
    
    std::string message;
    int shift;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Enter a message to encrypt (latin letters): ";
    std::getline(std::cin, message);
    
    std::cout << "Enter shift value (integer): ";
    std::cin >> shift;

    try {
        std::cout << "\n[Running encryption...]\n";
        TuringMachine encryptor = CreateCaesarMachine(shift);
        encryptor.LoadInput(message);
        encryptor.Run();
        
        std::string encryptedMessage = ExtractStringFromTape(encryptor.GetTape());
        std::cout << "-> Encrypted message: " << encryptedMessage << "\n";

        std::cout << "\n[Running decryption...]\n";
        TuringMachine decryptor = CreateCaesarMachine(-shift);
        decryptor.LoadInput(message);
        decryptor.Run();
        
        std::string decryptedMessage = ExtractStringFromTape(decryptor.GetTape());
        std::cout << "-> Decrypted message: " << decryptedMessage << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n[ERROR] Machine stopped unexpectedly!\n";
        std::cout << "Reason: " << e.what() << "\n";
        std::cout << "Make sure your input contains only standard ASCII characters.\n";
    }
}

//Для ленивых
int FibonacciRule(Sequence<int>* history) {
    if (history == nullptr || history->GetLength() < 2) {
        throw std::runtime_error("Fibonacci rule needs at least 2 elements in history");
    }
    int a = history->Get(history->GetLength() - 2);
    int b = history->Get(history->GetLength() - 1);

    if (a > 0 && b > std::numeric_limits<int>::max() - a) {
        throw std::overflow_error("Integer overflow detected in Fibonacci!");
    }

    return a + b;
}

int PowerOfTwoRule(Sequence<int>* history) {
    if (history == nullptr || history->GetLength() < 1) {
        throw std::runtime_error("Power rule needs at least 1 element in history");
    }
    int lastVal = history->Get(history->GetLength() - 1);

    if (lastVal > std::numeric_limits<int>::max() / 2) {
        throw std::overflow_error("Integer overflow detected in Powers of 2!");
    }

    return lastVal * 2;
}

int NaturalNumbersRule(Sequence<int>* history) {
    if (history == nullptr || history->GetLength() < 1) {
        throw std::runtime_error("Natural numbers rule needs at least 1 element in history");
    }
    int lastVal = history->Get(history->GetLength() - 1);
    if (lastVal == std::numeric_limits<int>::max()) {
        throw std::overflow_error("Integer overflow detected in Natural Numbers!");
    }
    return lastVal + 1;
}

// для ввода
int GetValidIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        } 
        else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[ERROR] Invalid input. Please enter a valid integer.\n";
        }
    }
}
//Ленивые 
void RunInteractiveDemo() {
    std::cout << "\n--- Interactive Lazy Sequence Demo ---\n";
    std::cout << "Select base sequence to start with:\n";
    std::cout << "1. Fibonacci sequence (1, 1, 2, 3, 5...)\n";
    std::cout << "2. Powers of 2 sequence (1, 2, 4, 8, 16...)\n";
    std::cout << "3. Natural numbers sequence (1, 2, 3, 4, 5...)\n";
    
    int baseChoice = GetValidIntInput("Select sequence type: ");
    
    Sequence<int>* currentSeq = nullptr;
    ArraySequence<int> initArr;

    if (baseChoice == 1) {
        initArr.Append(1);
        initArr.Append(1);
        currentSeq = new LazySequence<int>(FibonacciRule, &initArr);
        std::cout << "\nStarting with an infinite Fibonacci sequence...\n";
    } 
    else if (baseChoice == 2) {
        initArr.Append(1);
        currentSeq = new LazySequence<int>(PowerOfTwoRule, &initArr);
        std::cout << "\nStarting with an infinite Powers of 2 sequence...\n";
    }
    else if (baseChoice == 3) {
        initArr.Append(1);
        currentSeq = new LazySequence<int>(NaturalNumbersRule, &initArr);
        std::cout << "\nStarting with an infinite Natural numbers sequence...\n";
    }
    else {
        std::cout << "Invalid choice. Returning to menu.\n";
        return;
    }

    int currentHead = 0;

    bool running = true;
    while (running) {
        std::cout << "\n[Current Sequence Window (last 10/100 elements of the window)]\n";
        std::cout << "-> ";
        
        bool printedAnything = false;

        for (int i = currentHead; i < currentHead + 11; i++) {
            try { 
                int val = currentSeq->Get(i);
                std::cout << val << " ";
                printedAnything = true;
            }
            catch (const std::exception& e) {
                std::string errorMsg = e.what();
                if (!printedAnything) {
                    std::cout << "[Elements before index " << i << " are evicted from cache] ";
                }
                break;
            }
        }
        std::cout << "...\n";

        std::cout << "\nActions:\n";
        std::cout << "1. Append an element (to the end of Infinity)\n";
        std::cout << "2. Insert an element at index\n";
        std::cout << "3. Remove an element at index\n";
        std::cout << "4. Get element at index\n";
        std::cout << "5. Get element at Ordinal index\n";
        std::cout << "0. Return to Lazy menu\n";
        
        int subChoice = GetValidIntInput("Select action: ");

        try {
            if (subChoice == 0) {
                running = false;
            } 
            else if (subChoice == 1) {
                int val = GetValidIntInput("Enter integer to append: ");
                
                Sequence<int>* nextSeq = currentSeq->Append(val);
                delete currentSeq;
                currentSeq = nextSeq;
                
                std::cout << "Appended " << val << " to the sequence.\n";
            } 
            else if (subChoice == 2) {
                int val = GetValidIntInput("Enter integer to insert: ");
                
                int idx;
                while (true) {
                    idx = GetValidIntInput("Enter index for insertion: ");
                    if (idx >= 0) break;
                    std::cout << "Index must be >= 0.\n";
                }
                
                Sequence<int>* nextSeq = currentSeq->InsertAt(val, idx);
                delete currentSeq;
                currentSeq = nextSeq;
                
                std::cout << "Inserted " << val << " at index " << idx << ".\n";
            }
            else if (subChoice == 3) {
                int idx;
                while (true) {
                    idx = GetValidIntInput("Enter index to remove: ");
                    if (idx >= 0) break;
                    std::cout << "Index must be >= 0.\n";
                }
                
                auto lazySeq = dynamic_cast<LazySequence<int>*>(currentSeq);
                
                if (lazySeq != nullptr) {
                    try {
                        Sequence<int>* nextSeq = lazySeq->RemoveAt(Ordinal(idx)); 
                        
                        delete currentSeq;
                        currentSeq = nextSeq;
                        
                        std::cout << "Removed element at index " << idx << " from the sequence.\n";
                    } catch (const std::exception& e) {
                        std::cout << "[ERROR] " << e.what() << "\n";
                    }
                } 
                else {
                    std::cout << "[ERROR] Current sequence is not a LazySequence, cannot remove.\n";
                }
            }
            else if (subChoice == 4) {
                int idx;
                while (true) {
                    idx = GetValidIntInput("Enter index to retrieve: ");
                    if (idx >= 0) break;
                    std::cout << "Index must be >= 0.\n";
                }
                
                std::cout << "Element at index " << idx << " is: " << currentSeq->Get(idx) << "\n";
                
                if (idx > currentHead) {
                    int proposedHead = idx - 10;
                    if (proposedHead < 0) {
                        proposedHead = 0;
                    }
                    if (proposedHead > currentHead) {
                        currentHead = proposedHead;
                    }
                }
            }
            else if (subChoice == 5) {
                std::cout << "Select Ordinal type:\n";
                std::cout << "1. Finite (integer)\n";
                std::cout << "2. Omega + N (transfinite)\n";
                
                int ordType = GetValidIntInput("Choice: ");
                
                Ordinal targetOrd;
                
                if (ordType == 1) {
                    int idx;
                    while (true) {
                        idx = GetValidIntInput("Enter finite integer index: ");
                        if (idx >= 0) break;
                        std::cout << "Index must be >= 0.\n";
                    }
                    if (idx > currentHead) {
                        int proposedHead = idx - 10;
                        if (proposedHead < 0) {
                            proposedHead = 0;
                        }
                        if (proposedHead > currentHead) {
                            currentHead = proposedHead;
                        }
                    }
                    targetOrd = Ordinal(idx);
                } 
                else if (ordType == 2) {
                    int n;
                    while (true) {
                        n = GetValidIntInput("Enter N for (Omega + N) (e.g., 0 for just Omega): ");
                        if (n >= 0) break;
                        std::cout << "N must be >= 0.\n";
                    }
                    targetOrd = Ordinal::Omega() + Ordinal(n);
                } 
                else {
                    std::cout << "Invalid Ordinal type.\n";
                    continue;
                }
                
                auto lazySeq = dynamic_cast<LazySequence<int>*>(currentSeq);
                
                if (lazySeq != nullptr) {
                    std::cout << "Element at requested Ordinal index is: " << lazySeq->Get(targetOrd) << "\n";
                } else {
                    std::cout << "[ERROR] Current sequence is not a LazySequence, cannot use Ordinal Get.\n";
                }
            }
            else {
                std::cout << "Unknown action.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }

    delete currentSeq;
}

//вывод ленивой
void PrintSequence(LazySequence<int>& seq, int startIndex, int count, const std::string& name) {
    std::cout << name << " [indices " << startIndex << ".." << startIndex + count - 1 << "]: ";
    try {
        for (int i = startIndex; i < startIndex + count; i++) {
            std::cout << seq.Get(i) << (i < startIndex + count - 1 ? "  " : "");
        }
    } catch (const std::exception& e) {
        std::cout << "[Stop: " << e.what() << "]";
    }
    std::cout << "\n";
}

void RunLazySequenceUI() {
    std::cout << "\n--- Lazy Sequences ---\n";
    std::cout << "1. Fibonacci sequence\n";
    std::cout << "2. Powers of 2 sequence\n";
    std::cout << "3. Transfinite Maths Demo\n";
    std::cout << "4. Interactive LazySequence Demo (Append/Insert/Remove)\n";
    std::cout << "Select option: ";
    
    int subChoice;
    if (!(std::cin >> subChoice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    try {
        if (subChoice == 1 || subChoice == 2) {
            std::cout << "Enter start index (e.g., 0): ";
            int startIndex;
            std::cin >> startIndex;

            std::cout << "How many elements to show? (e.g., 10): ";
            int count;
            std::cin >> count;

            if (count <= 0 || startIndex < 0) {
                std::cout << "Invalid range.\n";
                return;
            }

            if (subChoice == 1) {
                std::cout << "\n[Generating Fibonacci Sequence]\n";
                ArraySequence<int> initFib;
                initFib.Append(1);
                initFib.Append(1);
                LazySequence<int> fib(FibonacciRule, &initFib);
                
                PrintSequence(fib, startIndex, count, "Fibonacci");
            } 
            else if (subChoice == 2) {
                std::cout << "\n[Generating Powers of 2 Sequence]\n";
                ArraySequence<int> initPow;
                initPow.Append(1);
                LazySequence<int> powSeq(PowerOfTwoRule, &initPow);
                
                PrintSequence(powSeq, startIndex, count, "Powers of 2");
            }
        }
        else if (subChoice == 3) {
            std::cout << "\n[Demonstrating (Immutable) Lazy Operations]\n\n";
            
            ArraySequence<int> initPow;
            initPow.Append(1);
            LazySequence<int> powers(PowerOfTwoRule, &initPow, 3); // 1, 2, 4
            PrintSequence(powers, 0, 3, "Seq A (Powers of 2)");

            ArraySequence<int> initFib;
            initFib.Append(1);
            initFib.Append(1);
            LazySequence<int> fibos(FibonacciRule, &initFib, 4); //1, 1, 2, 3
            PrintSequence(fibos, 0, 4, "Seq B (Fibonacci)  ");

            // (Seq A + Seq B)
            std::cout << "\nOperation: Concatenation (Seq A, Seq B)\n";
            LazySequence<int>* concatSeq = powers.Concat(fibos);
    
            PrintSequence(*concatSeq, 0, 7, "Result (Concat)    ");

            // insert
            std::cout << "\nOperation: Insertion (Seq B into Seq A at index 2)\n";
            LazySequence<int>* insertSeq = powers.Insert(fibos, Ordinal(2));
   
            PrintSequence(*insertSeq, 0, 7, "Result (Insert)    ");

            std::cout << "\nDirect access examples:\n";
            std::cout << "Concat[4] (should be Fib's 2nd element '1'): " << concatSeq->Get(4) << "\n";
            std::cout << "Insert[6] (should be Pow's 3rd element '4'): " << insertSeq->Get(6) << "\n";

            delete concatSeq;
            delete insertSeq;
        } 
        else if (subChoice == 4) {
            RunInteractiveDemo();
        }
    } catch (const std::exception& e) {
        std::cout << "\n[ERROR] Operation stopped!\nReason: " << e.what() << "\n";
    }
}

//потоки
void RunStreamsDemo() {
    std::cout << "\n--- Data Streams (File I/O) Demo ---\n";
    std::string filename = "test_stream.txt";
    
    std::ofstream touch(filename, std::ios::app);
    touch.close();

    FileInputStream<int>* inStreamInt = nullptr;
    FileOutputStream<int>* outStreamInt = nullptr;

    FileInputStream<std::string>* inStreamStr = nullptr;
    FileOutputStream<std::string>* outStreamStr = nullptr;

    IntSerializer intSerializer;
    IntDeserializer intDeserializer;
    StringSerializer strSerializer;
    StringDeserializer strDeserializer;

    bool running = true;
    while (running) {
        std::cout << "\nFile: " << filename << "\n";
        std::cout << "Actions:\n";
        std::cout << "--- Integer Streams ---\n";
        std::cout << "1. Write integer to file\n";
        std::cout << "2. Write multiple integers (0 to N)\n";
        std::cout << "3. Read NEXT integer from file\n";
        std::cout << "4. Read ALL remaining integers\n";
        std::cout << "--- String Streams ---\n";
        std::cout << "5. Write string to file\n";
        std::cout << "6. Read NEXT string from file\n";
        std::cout << "7. Read ALL remaining strings\n";
        std::cout << "--- General ---\n";
        std::cout << "8. Reset reading position to start\n";
        std::cout << "9. Clear file (Truncate)\n";
        std::cout << "0. Return to main menu\n";
        
        int subChoice = GetValidIntInput("Select action: ");

        try {
            if (subChoice == 0) {
                running = false;
            } 
            //int
            else if (subChoice == 1) {
                int val = GetValidIntInput("Enter integer to write: ");
                outStreamInt = new FileOutputStream<int>(filename, &intSerializer);

                outStreamInt->Open();
                outStreamInt->Write(val);
                outStreamInt->Close();

                delete outStreamInt;
                outStreamInt = nullptr;
                std::cout << "Written integer " << val << " to stream.\n";
            }
            else if (subChoice == 2) {
                int count;
                while (true) {
                    count = GetValidIntInput("How many numbers to write? (0 to N-1): ");
                    if (count > 0) break;
                    std::cout << "Must be > 0.\n";
                }

                outStreamInt = new FileOutputStream<int>(filename, &intSerializer);
                outStreamInt->Open();
                for (int i = 0; i < count; i++) {
                    outStreamInt->Write(i);
                }

                outStreamInt->Close();
                delete outStreamInt;
                outStreamInt = nullptr;
                std::cout << "Written " << count << " numbers to stream.\n";
            }
            else if (subChoice == 3) {
                if (inStreamInt == nullptr) {
                    inStreamInt = new FileInputStream<int>(filename, &intDeserializer);
                    inStreamInt->Open();
                }
                if (inStreamInt->IsEndOfStream()) {
                    std::cout << "End of stream reached. Use Reset (8) to read again.\n";
                } else {
                    int val = inStreamInt->Read();
                    std::cout << "Read integer: " << val << "\n";
                }
            }
            else if (subChoice == 4) {
                if (inStreamInt == nullptr) {
                    inStreamInt = new FileInputStream<int>(filename, &intDeserializer);
                    inStreamInt->Open();
                }

                std::cout << "Reading remaining ints: ";
                int count = 0;
                while (!inStreamInt->IsEndOfStream()) {
                    std::cout << inStreamInt->Read() << " ";
                    count++;
                }

                if (count == 0) std::cout << "(empty)";
                std::cout << "\n";
            }
            //string
            else if (subChoice == 5) {
                std::string val;
                std::cout << "Enter string to write (without spaces): ";
                std::cin >> val;
                
                outStreamStr = new FileOutputStream<std::string>(filename, &strSerializer);
                outStreamStr->Open();
                outStreamStr->Write(val);
                outStreamStr->Close();

                delete outStreamStr;
                outStreamStr = nullptr;
                std::cout << "Written string \"" << val << "\" to stream.\n";
            }
            else if (subChoice == 6) {
                if (inStreamStr == nullptr) {
                    inStreamStr = new FileInputStream<std::string>(filename, &strDeserializer);
                    inStreamStr->Open();
                }
                if (inStreamStr->IsEndOfStream()) {
                    std::cout << "End of stream reached. Use Reset (8) to read again.\n";
                } 
                else {
                    std::string val = inStreamStr->Read();
                    std::cout << "Read string: \"" << val << "\"\n";
                }
            }
            else if (subChoice == 7) {
                if (inStreamStr == nullptr) {
                    inStreamStr = new FileInputStream<std::string>(filename, &strDeserializer);
                    inStreamStr->Open();
                }

                std::cout << "Reading remaining strings: ";
                int count = 0;
                while (!inStreamStr->IsEndOfStream()) {
                    std::cout << "\"" << inStreamStr->Read() << "\" ";
                    count++;
                }

                if (count == 0) std::cout << "(empty)";
                std::cout << "\n";
            }
            //general
            else if (subChoice == 8) {
                if (inStreamInt != nullptr) {
                    inStreamInt->Close();
                    delete inStreamInt;
                    inStreamInt = nullptr;
                }
                if (inStreamStr != nullptr) {
                    inStreamStr->Close();
                    delete inStreamStr;
                    inStreamStr = nullptr;
                }
                std::cout << "Reading position reset to the beginning of the file.\n";
            }
            else if (subChoice == 9) {
                if (inStreamInt != nullptr) { 
                    inStreamInt->Close();
                    delete inStreamInt;
                    inStreamInt = nullptr;
                }
                if (inStreamStr != nullptr) { 
                    inStreamStr->Close();
                    delete inStreamStr;
                    inStreamStr = nullptr;
                }
                
                std::ofstream truncFile(filename, std::ios::trunc);
                truncFile.close();
                std::cout << "File cleared.\n";
            }
            else {
                std::cout << "Unknown action.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
    }

    if (inStreamInt != nullptr) { 
        inStreamInt->Close();
        delete inStreamInt;
    }
    if (outStreamInt != nullptr) { 
        outStreamInt->Close();
        delete outStreamInt;
    }
    if (inStreamStr != nullptr) { 
        inStreamStr->Close();
        delete inStreamStr;
    }
    if (outStreamStr != nullptr) { 
        outStreamStr->Close();
        delete outStreamStr;
    }
}

int main() {
    int choice = -1;
    
    while (choice != 0) {
        PrintMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                RunTuringMachineUI();
                break;
            case 2:
                RunLazySequenceUI();
                break;
            case 3:
                RunStreamsDemo();
                break;
            case 0:
                std::cout << "Exiting program...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    }

    return 0;
}