#include <gtest/gtest.h>
#include <limits>
#include "../LazySequence.hpp"
#include "../ArraySequence.hpp"
#include "../Exceptions.hpp"
#include "../Ordinal.hpp"

int TestFibRule(Sequence<int>* h) {
    int len = h->GetLength();
    return h->Get(len - 2) + h->Get(len - 1);
}

int PowerRule(Sequence<int>* h) {
    return h->Get(h->GetLength() - 1) * 2;
}

int IncrementalRule(Sequence<int>* h) {
    return h->Get(h->GetLength() - 1) + 1;
}

int ConstantRule(Sequence<int>* h) {
    return 19;
}

//генерация
TEST(LazySequenceTest, FibonacciGeneration) {
    ArraySequence<int> init; 
    init.Append(1); 
    init.Append(1);
    LazySequence<int> seq(TestFibRule, &init);

    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(6), 13);
}

//Пограничные значения и исключения
TEST(LazySequenceTest, NegativeIndexThrows) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init);
    
    EXPECT_THROW(seq.Get(-1), IndexOutOfRangeException);
    EXPECT_THROW(seq.Get(-100), IndexOutOfRangeException);
}

TEST(LazySequenceTest, LargeIndexAccess) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init);
    
    EXPECT_NO_THROW(seq.Get(10000));
    EXPECT_EQ(seq.Get(10000), 10001);
}

//Тестирование append, prepend, insert и трансфинитных индексов
TEST(LazySequenceTest, AppendOperation) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init);
    
    Sequence<int>* appended = seq.Append(999);
    auto lazyAppended = dynamic_cast<LazySequence<int>*>(appended);
    ASSERT_NE(lazyAppended, nullptr);
    
    EXPECT_EQ(lazyAppended->Get(0), 1);
    EXPECT_EQ(lazyAppended->Get(5), 6);
    EXPECT_EQ(lazyAppended->Get(Ordinal::Omega()), 999);
    
    delete appended;
}

TEST(LazySequenceTest, AppendMultiple) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init);
    
    Sequence<int>* s1 = seq.Append(100);
    Sequence<int>* s2 = s1->Append(200);
    Sequence<int>* s3 = s2->Append(300);
    auto lazy = dynamic_cast<LazySequence<int>*>(s3);
    
    EXPECT_EQ(lazy->Get(Ordinal::Omega()), 100);
    EXPECT_EQ(lazy->Get(Ordinal::Omega() + Ordinal(1)), 200);
    EXPECT_EQ(lazy->Get(Ordinal::Omega() + Ordinal(2)), 300);
    
    delete s1;
    delete s2;
    delete s3;
}

TEST(LazySequenceTest, PrependOperation) {
    ArraySequence<int> init; 
    init.Append(5);
    init.Append(6);
    LazySequence<int> seq(ConstantRule, &init, 2);
    
    Sequence<int>* prepended = seq.Prepend(999);
    auto lazyPrepended = dynamic_cast<LazySequence<int>*>(prepended);
    
    EXPECT_EQ(lazyPrepended->Get(0), 999);
    EXPECT_EQ(lazyPrepended->Get(1), 5);
    EXPECT_EQ(lazyPrepended->Get(2), 6);
    EXPECT_THROW(lazyPrepended->Get(3), IndexOutOfRangeException);
    
    delete prepended;
}

TEST(LazySequenceTest, InsertAtOperation) {
    ArraySequence<int> init; 
    init.Append(1);
    init.Append(2);
    init.Append(3);
    LazySequence<int> seq(ConstantRule, &init, 3);
    
    Sequence<int>* inserted = seq.InsertAt(999, 1);
    auto lazyInserted = dynamic_cast<LazySequence<int>*>(inserted);
    
    EXPECT_EQ(lazyInserted->Get(0), 1);
    EXPECT_EQ(lazyInserted->Get(1), 999);
    EXPECT_EQ(lazyInserted->Get(2), 2);
    EXPECT_EQ(lazyInserted->Get(3), 3);
    
    delete inserted;
}

TEST(LazySequenceTest, InsertAtBeginning) {
    ArraySequence<int> init; 
    init.Append(10);
    init.Append(20);
    LazySequence<int> seq(ConstantRule, &init, 2);
    
    Sequence<int>* inserted = seq.InsertAt(999, 0);
    auto lazyInserted = dynamic_cast<LazySequence<int>*>(inserted);
    
    EXPECT_EQ(lazyInserted->Get(0), 999);
    EXPECT_EQ(lazyInserted->Get(1), 10);
    EXPECT_EQ(lazyInserted->Get(2), 20);
    
    delete inserted;
}

TEST(LazySequenceTest, InsertAtEnd) {
    ArraySequence<int> init; 
    init.Append(1);
    init.Append(2);
    LazySequence<int> seq(ConstantRule, &init, 2);
    
    Sequence<int>* inserted = seq.InsertAt(999, 2);
    auto lazyInserted = dynamic_cast<LazySequence<int>*>(inserted);
    
    EXPECT_EQ(lazyInserted->Get(0), 1);
    EXPECT_EQ(lazyInserted->Get(1), 2);
    EXPECT_EQ(lazyInserted->Get(2), 999);
    
    delete inserted;
}

TEST(LazySequenceTest, InsertInvalidIndex) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(ConstantRule, &init);
    
    EXPECT_THROW(seq.InsertAt(999, -1), IndexOutOfRangeException);
}

//Remove
TEST(LazySequenceTest, RemoveAtOperation) {
    ArraySequence<int> init; 
    init.Append(0);
    init.Append(1);
    init.Append(2);
    init.Append(3);
    LazySequence<int> seq(ConstantRule, &init, 4);
    
    Sequence<int>* removed = seq.RemoveAt(Ordinal(2));
    auto lazyRemoved = dynamic_cast<LazySequence<int>*>(removed);
    
    EXPECT_EQ(lazyRemoved->Get(0), 0);
    EXPECT_EQ(lazyRemoved->Get(1), 1);
    EXPECT_EQ(lazyRemoved->Get(2), 3);
    EXPECT_THROW(lazyRemoved->Get(3), IndexOutOfRangeException);
    
    delete removed;
}

TEST(LazySequenceTest, RemoveFirstElement) {
    ArraySequence<int> init; 
    init.Append(10);
    init.Append(20);
    init.Append(30);
    LazySequence<int> seq(ConstantRule, &init, 3);
    
    Sequence<int>* removed = seq.RemoveAt(Ordinal(0));
    auto lazyRemoved = dynamic_cast<LazySequence<int>*>(removed);
    
    EXPECT_EQ(lazyRemoved->Get(0), 20);
    EXPECT_EQ(lazyRemoved->Get(1), 30);
    
    delete removed;
}

TEST(LazySequenceTest, RemoveLastElement) {
    ArraySequence<int> init; 
    init.Append(10);
    init.Append(20);
    init.Append(30);
    LazySequence<int> seq(ConstantRule, &init, 3);
    
    Sequence<int>* removed = seq.RemoveAt(Ordinal(2));
    auto lazyRemoved = dynamic_cast<LazySequence<int>*>(removed);
    
    EXPECT_EQ(lazyRemoved->Get(0), 10);
    EXPECT_EQ(lazyRemoved->Get(1), 20);
    EXPECT_THROW(lazyRemoved->Get(2), IndexOutOfRangeException);
    
    delete removed;
}

TEST(LazySequenceTest, RemoveFromInfiniteSequence) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init);
    
    Sequence<int>* removed = seq.RemoveAt(Ordinal(5));
    auto lazyRemoved = dynamic_cast<LazySequence<int>*>(removed);
    
    EXPECT_EQ(lazyRemoved->Get(0), 1);
    EXPECT_EQ(lazyRemoved->Get(4), 5);
    EXPECT_EQ(lazyRemoved->Get(5), 7);
    EXPECT_EQ(lazyRemoved->Get(6), 8);
    
    EXPECT_TRUE(lazyRemoved->GetOrdinalLength().IsInfinite());
    
    delete removed;
}

//Concat
TEST(LazySequenceTest, ConcatTwoFiniteSequences) {
    ArraySequence<int> init1; 
    init1.Append(1);
    init1.Append(2);
    LazySequence<int> seq1(ConstantRule, &init1, 2);
    
    ArraySequence<int> init2; 
    init2.Append(10);
    init2.Append(20);
    LazySequence<int> seq2(ConstantRule, &init2, 2);
    
    LazySequence<int>* concat = seq1.Concat(seq2);
    
    EXPECT_EQ(concat->GetLength(), 4);
    EXPECT_EQ(concat->Get(0), 1);
    EXPECT_EQ(concat->Get(1), 2);
    EXPECT_EQ(concat->Get(2), 10);
    EXPECT_EQ(concat->Get(3), 20);
    
    delete concat;
}

TEST(LazySequenceTest, ConcatFiniteWithInfinite) {
    ArraySequence<int> initFin; 
    initFin.Append(100);
    initFin.Append(200);
    LazySequence<int> finite(ConstantRule, &initFin, 2);
    
    ArraySequence<int> initInf; 
    initInf.Append(1);
    LazySequence<int> infinite(IncrementalRule, &initInf);
    
    LazySequence<int>* concat = finite.Concat(infinite);
    
    EXPECT_EQ(concat->GetLength(), -1);
    EXPECT_EQ(concat->Get(0), 100);
    EXPECT_EQ(concat->Get(1), 200);
    EXPECT_EQ(concat->Get(2), 1);
    EXPECT_EQ(concat->Get(3), 2);
    EXPECT_EQ(concat->Get(100), 99);
    
    delete concat;
}

TEST(LazySequenceTest, ConcatInfiniteWithFinite) {
    ArraySequence<int> initInf; 
    initInf.Append(1);
    LazySequence<int> infinite(IncrementalRule, &initInf);
    
    ArraySequence<int> initFin; 
    initFin.Append(999);
    LazySequence<int> finite(ConstantRule, &initFin, 1);
    
    LazySequence<int>* concat = infinite.Concat(finite);
    
    EXPECT_TRUE(concat->GetOrdinalLength().IsInfinite());
    EXPECT_EQ(concat->Get(0), 1);
    EXPECT_EQ(concat->Get(100), 101);
    EXPECT_EQ(concat->Get(Ordinal::Omega()), 999);
    
    delete concat;
}

//Пустая
TEST(LazySequenceTest, EmptySequence) {
    LazySequence<int> emptySeq;
    
    EXPECT_THROW(emptySeq.Get(0), IndexOutOfRangeException);
    EXPECT_THROW(emptySeq.GetFirst(), IndexOutOfRangeException);
    EXPECT_EQ(emptySeq.GetLength(), 0);
}

//Ограничение генератора
TEST(LazySequenceTest, FiniteSequence) {
    ArraySequence<int> init; 
    init.Append(1);
    init.Append(2);
    init.Append(3);
    LazySequence<int> seq(ConstantRule, &init, 3);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
    EXPECT_THROW(seq.Get(3), IndexOutOfRangeException);
}

TEST(LazySequenceTest, FiniteWithGeneratorRule) {
    ArraySequence<int> init; 
    init.Append(1);
    LazySequence<int> seq(IncrementalRule, &init, 5);
    
    EXPECT_EQ(seq.GetLength(), 5);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
    EXPECT_EQ(seq.Get(3), 4);
    EXPECT_EQ(seq.Get(4), 5);
    EXPECT_THROW(seq.Get(5), IndexOutOfRangeException);
}

TEST(LazySequenceTest, InsertSequence) {
    ArraySequence<int> init1; 
    init1.Append(1);
    init1.Append(2);
    LazySequence<int> seq1(ConstantRule, &init1, 2);
    
    ArraySequence<int> init2; 
    init2.Append(10);
    init2.Append(20);
    init2.Append(30);
    LazySequence<int> seq2(ConstantRule, &init2, 3);
    
    LazySequence<int>* inserted = seq1.Insert(seq2, Ordinal(1));
    
    EXPECT_EQ(inserted->Get(0), 1);
    EXPECT_EQ(inserted->Get(1), 10);
    EXPECT_EQ(inserted->Get(2), 20);
    EXPECT_EQ(inserted->Get(3), 30);
    EXPECT_EQ(inserted->Get(4), 2);
    EXPECT_EQ(inserted->GetLength(), 5);
    
    delete inserted;
}

TEST(LazySequenceTest, InsertSequenceAtBeginning) {
    ArraySequence<int> init1; 
    init1.Append(100);
    init1.Append(200);
    LazySequence<int> seq1(ConstantRule, &init1, 2);
    
    ArraySequence<int> init2; 
    init2.Append(1);
    init2.Append(2);
    init2.Append(3);
    LazySequence<int> seq2(ConstantRule, &init2, 3);
    
    LazySequence<int>* inserted = seq1.Insert(seq2, Ordinal(0));
    
    EXPECT_EQ(inserted->Get(0), 1);
    EXPECT_EQ(inserted->Get(1), 2);
    EXPECT_EQ(inserted->Get(2), 3);
    EXPECT_EQ(inserted->Get(3), 100);
    EXPECT_EQ(inserted->Get(4), 200);
    
    delete inserted;
}

//ординалы
TEST(OrdinalTest, ArithmeticAndComparisons) {
    Ordinal zero(0);
    Ordinal five(5);
    Ordinal omega = Ordinal::Omega(); // ω
    Ordinal omegaPlusFive = omega + five; // ω + 5

    EXPECT_LT(five, omega);
    EXPECT_GT(omegaPlusFive, omega);
    EXPECT_EQ(omegaPlusFive.GetOmegaPart(), 1);
    EXPECT_EQ(omegaPlusFive.GetFinitePart(), 5);

    Ordinal fivePlusOmega = five + omega;
    EXPECT_EQ(fivePlusOmega, omega); 
    EXPECT_NE(omega + five, omega);

    // Левое вычитание: (ω + 5) - ω = 5
    EXPECT_EQ(Ordinal::SubtractLeft(omega, omegaPlusFive), Ordinal(5));
}

TEST(OrdinalTest, InvalidArgumentsThrow) {
    EXPECT_THROW(Ordinal(-5), InvalidArgumentException);
    EXPECT_THROW(Ordinal(-1, 0), InvalidArgumentException);
    EXPECT_THROW(Ordinal::SubtractLeft(Ordinal(5), Ordinal(3)), InvalidArgumentException);
}

//конкатенация бесконечностей
TEST(LazySequenceTest, ConcatTwoInfiniteSequences) {
    ArraySequence<int> init1; 
    init1.Append(1);
    LazySequence<int> naturals(IncrementalRule, &init1);
    
    ArraySequence<int> init2; 
    init2.Append(1);
    LazySequence<int> powers(PowerRule, &init2);
    
    LazySequence<int>* concat = naturals.Concat(powers);
    
    EXPECT_TRUE(concat->GetOrdinalLength().IsInfinite());
    EXPECT_EQ(concat->GetOrdinalLength().GetOmegaPart(), 2);
    EXPECT_EQ(concat->GetOrdinalLength().GetFinitePart(), 0);
    
    EXPECT_EQ(concat->Get(0), 1);
    EXPECT_EQ(concat->Get(1), 2);
    EXPECT_EQ(concat->Get(2), 3);
    EXPECT_EQ(concat->Get(10), 11);
    
    EXPECT_EQ(concat->Get(Ordinal::Omega()), 1);
    EXPECT_EQ(concat->Get(Ordinal::Omega() + Ordinal(1)), 2);
    EXPECT_EQ(concat->Get(Ordinal::Omega() + Ordinal(2)), 4);
    EXPECT_EQ(concat->Get(Ordinal::Omega() + Ordinal(3)), 8);
    EXPECT_EQ(concat->Get(Ordinal::Omega() + Ordinal(10)), 1024);
    
    EXPECT_EQ(concat->Get(100), 101);
    
    delete concat;
}

TEST(LazySequenceTest, ConcatInfiniteWithInfiniteFibonacciAndPowers) {
    ArraySequence<int> initFib; 
    initFib.Append(1);
    initFib.Append(1);
    LazySequence<int> fibonacci(TestFibRule, &initFib);
    
    ArraySequence<int> initPow; 
    initPow.Append(1);
    LazySequence<int> powers(PowerRule, &initPow);
    
    LazySequence<int>* concat = fibonacci.Concat(powers);
    
    EXPECT_EQ(concat->Get(0), 1);
    EXPECT_EQ(concat->Get(1), 1);
    EXPECT_EQ(concat->Get(2), 2);
    EXPECT_EQ(concat->Get(3), 3);
    EXPECT_EQ(concat->Get(4), 5);
    EXPECT_EQ(concat->Get(5), 8);
    EXPECT_EQ(concat->Get(6), 13);
    
    Ordinal omega = Ordinal::Omega();
    EXPECT_EQ(concat->Get(omega), 1);
    EXPECT_EQ(concat->Get(omega + Ordinal(1)), 2);
    EXPECT_EQ(concat->Get(omega + Ordinal(2)), 4);
    EXPECT_EQ(concat->Get(omega + Ordinal(3)), 8);
    EXPECT_EQ(concat->Get(omega + Ordinal(4)), 16);
    EXPECT_EQ(concat->Get(omega + Ordinal(5)), 32);
    
    delete concat;
}

TEST(LazySequenceTest, ConcatMultipleInfiniteSequences) {
    ArraySequence<int> initNat; 
    initNat.Append(1);
    LazySequence<int> naturals(IncrementalRule, &initNat);
    
    ArraySequence<int> initPow; 
    initPow.Append(1);
    LazySequence<int> powers(PowerRule, &initPow);
    
    ArraySequence<int> initFib; 
    initFib.Append(1);
    initFib.Append(1);
    LazySequence<int> fibonacci(TestFibRule, &initFib);
    
    LazySequence<int>* step1 = naturals.Concat(powers);
    LazySequence<int>* concat = step1->Concat(fibonacci);
    
    EXPECT_TRUE(concat->GetOrdinalLength().IsInfinite());
    EXPECT_EQ(concat->GetOrdinalLength().GetOmegaPart(), 3);
    
    EXPECT_EQ(concat->Get(0), 1);
    EXPECT_EQ(concat->Get(50), 51);
    
    Ordinal omega = Ordinal::Omega();
    EXPECT_EQ(concat->Get(omega), 1);
    EXPECT_EQ(concat->Get(omega + Ordinal(5)), 32);
    
    Ordinal omega2 = omega + omega;
    EXPECT_EQ(concat->Get(omega2), 1);
    EXPECT_EQ(concat->Get(omega2 + Ordinal(1)), 1);
    EXPECT_EQ(concat->Get(omega2 + Ordinal(2)), 2);
    EXPECT_EQ(concat->Get(omega2 + Ordinal(3)), 3);
    EXPECT_EQ(concat->Get(omega2 + Ordinal(4)), 5);
    
    delete step1;
    delete concat;
}

//Insert
TEST(LazySequenceTest, InsertInfiniteSequenceIntoInfinite) {
    ArraySequence<int> initBase; 
    initBase.Append(1);
    LazySequence<int> base(IncrementalRule, &initBase); //1,2,3,4...
    
    ArraySequence<int> initInsert; 
    initInsert.Append(1);
    LazySequence<int> toInsert(PowerRule, &initInsert); //1,2,4,8,16...
    
    //1,2,3,4,5, 1,2,4,8,16..., 6,7,8,9,10,11...
    LazySequence<int>* inserted = base.Insert(toInsert, Ordinal(5));
    
    EXPECT_TRUE(inserted->GetOrdinalLength().IsInfinite());
    EXPECT_EQ(inserted->GetOrdinalLength().GetOmegaPart(), 2);
    
    EXPECT_EQ(inserted->Get(0), 1);
    EXPECT_EQ(inserted->Get(1), 2);
    EXPECT_EQ(inserted->Get(2), 3);
    EXPECT_EQ(inserted->Get(3), 4);
    EXPECT_EQ(inserted->Get(4), 5);
    
    EXPECT_EQ(inserted->Get(5), 1);
    EXPECT_EQ(inserted->Get(6), 2);
    EXPECT_EQ(inserted->Get(7), 4);
    EXPECT_EQ(inserted->Get(8), 8);
    EXPECT_EQ(inserted->Get(9), 16);
    EXPECT_EQ(inserted->Get(100), 0);
    
    Ordinal omega = Ordinal::Omega();
    EXPECT_EQ(inserted->Get(omega + Ordinal(5)), 11);
    
    delete inserted;
}