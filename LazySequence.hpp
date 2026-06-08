#ifndef _LAZYSEQUENCE_HPP_
#define _LAZYSEQUENCE_HPP_

#include <functional>
#include "Sequence.hpp"
#include "Ordinal.hpp"
#include "LazyNode.hpp"
#include "SingleValueNode.hpp"
#include "SourceNode.hpp"
#include "ConcatNode.hpp"
#include "InsertNode.hpp"
#include "RemoveNode.hpp"
#include "Generator.hpp"

template <typename T> class LazySequence;

template <typename T>
class LazyEnumerator : public IEnumerator<T> {
private:
    const LazySequence<T>* sequence;
    int currentIndex;
    T currentValue;
    bool hasCurrent;

public:
    explicit LazyEnumerator(const LazySequence<T>* seq)
        : sequence(seq), currentIndex(0), hasCurrent(false) {}

    bool MoveNext() override {
        try {
            currentValue = sequence->Get(currentIndex);
            hasCurrent = true;
            currentIndex++;
            return true;
        } catch (const IndexOutOfRangeException&) {
            hasCurrent = false;
            return false;
        }
    }

    T& GetCurrent() override {
        if (!hasCurrent) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return currentValue; 
    }

    const T& GetCurrent() const override {
        if (!hasCurrent) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return currentValue; 
    }

    void Reset() override {
        currentIndex = 0;
        hasCurrent = false;
    }
    
    IEnumerator<T>* Clone() const override {
        auto copy = new LazyEnumerator<T>(sequence);
        copy->currentIndex = currentIndex;
        copy->hasCurrent = hasCurrent;
        if (hasCurrent) {
            copy->currentValue = currentValue;
        }
        return copy;
    }
};

template <typename T>
class LazySequence : public Sequence<T> {
private:
    LazyNode<T>* root;

    explicit LazySequence(LazyNode<T>* node) : root(node) {
        if (node == nullptr) throw InvalidArgumentException("Root node cannot be null");
    }

protected:
    Sequence<T>* CreateEmptySameType() const override {
        return new LazySequence<T>();
    }

    void AppendInternal(const T& item) override {
        LazyNode<T>* oldRoot = root;
        root = new ConcatNode<T>(oldRoot, new SingleValueNode<T>(item));
    }

    void PrependInternal(const T& item) override {
        LazyNode<T>* oldRoot = root;
        root = new InsertNode<T>(oldRoot, new SingleValueNode<T>(item), Ordinal(0));
    }

    void InsertAtInternal(const T& item, int index) override {
        if (index < 0) throw IndexOutOfRangeException("Negative index");
        LazyNode<T>* oldRoot = root;
        root = new InsertNode<T>(oldRoot, new SingleValueNode<T>(item), Ordinal(index));
    }

    void SetInternal(int index, const T& item) override {
        throw UnsupportedOperationException("Set is not supported for immutable LazySequence");
    }

public:
    LazySequence() {
        auto emptyRule = [](Sequence<T>*) -> T {
            throw UnsupportedOperationException("Empty sequence has no generator rule");
        };
        root = new SourceNode<T>(new RuleGenerator<T>(emptyRule, nullptr, 0), Ordinal(0), nullptr);
    }

    LazySequence(std::function<T(Sequence<T>*)> rule, Sequence<T>* initial = nullptr, int limit = -1, int windowSize = 100) {
        Ordinal len = (limit == -1) ? Ordinal::Omega() : Ordinal(limit);
        root = new SourceNode<T>(new RuleGenerator<T>(rule, initial, limit), len, initial, windowSize);
    }
    
    LazySequence(const LazySequence<T>& other) : root(other.root->Clone()) {}

    ~LazySequence() override { 
        delete root; 
    }

    LazySequence<T>& operator=(const LazySequence<T>& other) {
        if (this != &other) {
            delete root;
            root = other.root->Clone();
        }
        return *this;
    }

    T Get(const Ordinal& index) const {
        return root->Get(index);
    }

    T Get(int index) const override {
        if (index < 0) throw IndexOutOfRangeException("Negative index");
        return Get(Ordinal(index));
    }

    T GetFirst() const override { 
        return Get(0); 
    }

    T GetLast() const override {
        Ordinal len = root->GetLength();
        if (len.IsInfinite()) {
            throw UnsupportedOperationException("GetLast is undefined for transfinite lazy sequence");
        }

        int n = len.GetFinitePart();
        if (n == 0) throw EmptyContainerException("LazySequence is empty");
        return Get(n - 1);
    }

    int GetLength() const override {
        Ordinal len = root->GetLength();
        if (len.IsInfinite()) return -1; // -1 = infinite
        return len.GetFinitePart();
    }

    Ordinal GetOrdinalLength() const {
        return root->GetLength();
    }

    LazySequence<T>* Concat(const LazySequence<T>& other) const {
        return new LazySequence<T>(new ConcatNode<T>(root->Clone(), other.root->Clone()));
    }

    LazySequence<T>* Insert(const LazySequence<T>& other, const Ordinal& position) const {
        return new LazySequence<T>(new InsertNode<T>(root->Clone(), other.root->Clone(), position));
    }

    LazySequence<T>* RemoveAt(const Ordinal& index) const {
        return new LazySequence<T>(new RemoveNode<T>(root->Clone(), index, Ordinal(1)));
    }

    LazySequence<T>* RemoveSequence(const Ordinal& index, const Ordinal& length) const {
        return new LazySequence<T>(new RemoveNode<T>(root->Clone(), index, length));
    }

    Sequence<T>* Clone() const override { 
        return new LazySequence<T>(*this); 
    }

    Sequence<T>* Instance() override { 
        return Clone(); 
    }
    
    T& operator[](int index) override { throw UnsupportedOperationException("Mutable access is not supported"); }
    const T& operator[](int index) const override { throw UnsupportedOperationException("Reference access is not supported"); }
    
    IEnumerator<T>* GetEnumerator() const override { 
        return new LazyEnumerator<T>(this); 
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = delete;
    Sequence<T>* Where(std::function<bool(const T&)> predicate) const = delete;
    template<typename Accumulator>
    Accumulator Reduce(const Accumulator& initial, std::function<Accumulator(Accumulator, const T&)> func) const = delete;
    Sequence<T>* Slice(int start, int count, Sequence<T>* replacement = nullptr) const = delete;
};

#endif // _LAZYSEQUENCE_HPP_