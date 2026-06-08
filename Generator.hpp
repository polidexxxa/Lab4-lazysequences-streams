#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include "IEnumerator.hpp"
#include "FixedQueueSequence.hpp"
#include "Option.hpp"
#include "Exceptions.hpp"
#include <functional>


template <typename T>
class Generator {
public:
    virtual ~Generator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual Option<T> TryGetNext() { return HasNext() ? Option<T>(GetNext()) : Option<T>(); }
    virtual Generator<T>* Clone() const = 0;
};


// RuleGenerator
template <typename T>
class RuleGenerator : public Generator<T> {
private:
    std::function<T(Sequence<T>*)> rule;
    int limit;
    int generatedCount;
    FixedQueueSequence<T>* historyQueue;

public:
    RuleGenerator(std::function<T(Sequence<T>*)> genRule, Sequence<T>* initial = nullptr, int limitValue = -1)
        : rule(genRule), limit(limitValue), generatedCount(0) {

        int historySize = initial ? initial->GetLength() : 1;
        if (historySize <= 0) {
            historySize = 1;
        }

        historyQueue = new FixedQueueSequence<T>(historySize);

        if (initial != nullptr) {
            IEnumerator<T>* enumerator = initial->GetEnumerator();
            while (enumerator->MoveNext()) {
                historyQueue->Enqueue(enumerator->GetCurrent());
            }
            delete enumerator;
        }
    }

    RuleGenerator(const RuleGenerator<T>& other)
        : rule(other.rule), 
          limit(other.limit), 
          generatedCount(other.generatedCount),
          historyQueue(static_cast<FixedQueueSequence<T>*>(other.historyQueue->Clone())) {}

    ~RuleGenerator() override { 
        delete historyQueue; 
    }

    bool HasNext() const override { 
        return limit == -1 || generatedCount < limit; 
    }

    T GetNext() override {
        if (!HasNext()) throw IndexOutOfRangeException("Generator exhausted");
        
        T value = rule(historyQueue);
        historyQueue->Enqueue(value);
        generatedCount++;
        
        return value;
    }

    Generator<T>* Clone() const override { 
        return new RuleGenerator<T>(*this); 
    }
};

#endif // _GENERATOR_HPP_