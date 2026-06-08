#ifndef TURINGTAPE_HPP
#define TURINGTAPE_HPP

#include "TuringTypes.hpp"
#include "ArraySequence.hpp"
#include <string>

class TuringTape {
private:
    ArraySequence<TuringSymbol> rightTape; 
    ArraySequence<TuringSymbol> leftTape;  
    TuringSymbol blankSymbol;

public:
    explicit TuringTape(TuringSymbol blank = '_') : blankSymbol(blank) {}

    TuringSymbol Read(int position) const {
        if (position >= 0) {
            if (position < rightTape.GetLength()) {
                return rightTape.Get(position);
            }
        } 
        else {
            int leftIndex = -position - 1;
            if (leftIndex < leftTape.GetLength()) {
                return leftTape.Get(leftIndex);
            }
        }
        return blankSymbol; 
    }

    void Write(int position, TuringSymbol symbol) {
        if (position >= 0) {
            while (position >= rightTape.GetLength()) {
                rightTape.Append(blankSymbol);
            }
            rightTape.Set(position, symbol);
        } 
        else {
            int leftIndex = -position - 1;
            while (leftIndex >= leftTape.GetLength()) {
                leftTape.Append(blankSymbol);
            }
            leftTape.Set(leftIndex, symbol);
        }
    }

    void SetInput(const std::string& input) {
        rightTape = ArraySequence<TuringSymbol>();
        leftTape = ArraySequence<TuringSymbol>();
        
        for (int i = 0; i < input.length(); i++) {
            Write(i, input[i]);
        }
    }

    int GetMinPosition() const {
        return -leftTape.GetLength();
    }

    int GetMaxPosition() const {
        return rightTape.GetLength() > 0 ? rightTape.GetLength() - 1 : 0;
    }
};

#endif // TURINGTAPE_HPP