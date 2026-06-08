#include "LazySequence.hpp"
#include "ArraySequence.hpp"
#include <functional>
#include <iostream>

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

int main(){
    ArraySequence<int> initArr;

    initArr.Append(1);
    auto testSeq = LazySequence<int>(NaturalNumbersRule, &initArr, 3); //после трех элементов HasNext - false

    try{
        for (int i = 0; i < 5; i++) {
            std::cout << testSeq.Get(i) << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "[Stop: " << e.what() << "]";
    }

    return 0;
}