#ifndef TURINGMACHINE_HPP
#define TURINGMACHINE_HPP

#include "TuringTypes.hpp"
#include "TuringTape.hpp"
#include "ArraySequence.hpp"
#include <stdexcept>
#include <string>

class TuringMachine {
private:
    TuringTape tape;
    ArraySequence<TuringRule> rules;
    
    TuringState initialState;
    TuringState haltState;
    
    TuringState currentState;
    int headPosition;

    bool FindRule(const TuringState& state, TuringSymbol symbol, TuringRule& outRule) const {
        for (int i = 0; i < rules.GetLength(); i++) {
            TuringRule rule = rules.Get(i);
            if (rule.currentState == state && rule.currentSymbol == symbol) {
                outRule = rule;
                return true;
            }
        }
        return false;
    }

public:
    TuringMachine(TuringState startState, TuringState endState, TuringSymbol blank = '_')
        : tape(blank), rules(), initialState(startState), haltState(endState), 
          currentState(startState), headPosition(0) {}

    void AddRule(const TuringRule& rule) {
        rules.Append(rule);
    }

    void LoadInput(const std::string& input) {
        tape.SetInput(input);
        currentState = initialState;
        headPosition = 0;
    }

    const TuringTape& GetTape() const { return tape; }
    TuringState GetCurrentState() const { return currentState; }
    int GetHeadPosition() const { return headPosition; }
    bool IsHalted() const { return currentState == haltState; }
   
    bool Step() {
        if (IsHalted()) {
            return false;
        }

        TuringSymbol currentSymbol = tape.Read(headPosition);
        TuringRule rule("", ' ', ' ', MoveDirection::None, "");

        if (!FindRule(currentState, currentSymbol, rule)) {
            throw std::runtime_error("No rule found for state '" + currentState + "' and symbol '" + std::string(1, currentSymbol) + "'");
        }

        tape.Write(headPosition, rule.writeSymbol);

        if (rule.move == MoveDirection::Left) {
            headPosition--;
        } 
        else if (rule.move == MoveDirection::Right) {
            headPosition++;
        }
        currentState = rule.nextState;
        
        return true;
    }

    void Run() {
        while (Step()) {}
    }
};

#endif // TURINGMACHINE_HPP