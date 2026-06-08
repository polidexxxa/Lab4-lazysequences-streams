#ifndef _TURINGTYPES_HPP_
#define _TURINGTYPES_HPP_

#include <string>
#include <functional>

enum class MoveDirection {
    Left,
    Right,
    None
};

using TuringState = std::string;
using TuringSymbol = char;

struct TuringRule {
    TuringState currentState;
    TuringSymbol currentSymbol;
    
    TuringSymbol writeSymbol;
    MoveDirection move;
    TuringState nextState;

    TuringRule() 
        : currentState(""), currentSymbol(' '), 
          writeSymbol(' '), move(MoveDirection::None), nextState("") {}

    TuringRule(TuringState currState, TuringSymbol currSym, 
               TuringSymbol wrSym, MoveDirection m, TuringState nxState)
        : currentState(currState), currentSymbol(currSym), 
          writeSymbol(wrSym), move(m), nextState(nxState) {}
};

#endif // _TURINGTYPES_HPP_