#include <gtest/gtest.h>
#include "../TuringMachine.hpp"

//Проверка базового состояния
TEST(TuringMachineTest, InitialState) {
    TuringMachine tm("q0", "halt", '_');
    EXPECT_EQ(tm.GetCurrentState(), "q0");
    EXPECT_EQ(tm.GetHeadPosition(), 0);
}

//Проверка шифрования (Цезарь со сдвигом 2)
TEST(TuringMachineTest, CaesarCipherShift2) {
    TuringMachine tm("encrypt", "halt", '_');
    tm.AddRule(TuringRule("encrypt", 'A', 'C', MoveDirection::Right, "encrypt"));
    tm.AddRule(TuringRule("encrypt", 'B', 'D', MoveDirection::Right, "encrypt"));
    tm.AddRule(TuringRule("encrypt", '_', '_', MoveDirection::None, "halt"));

    tm.LoadInput("AB");
    tm.Run();

    EXPECT_EQ(tm.GetTape().Read(0), 'C');
    EXPECT_EQ(tm.GetTape().Read(1), 'D');
    EXPECT_EQ(tm.GetCurrentState(), "halt");
}

//Пограничное значение: Огромная лента
TEST(TuringMachineTest, LargeTapeBoundary) {
    TuringMachine tm("start", "halt", '_');
    tm.AddRule(TuringRule("start", '1', '1', MoveDirection::Right, "start"));
    tm.AddRule(TuringRule("start", '_', '0', MoveDirection::None, "halt"));

    std::string largeInput(100000, '1');
    tm.LoadInput(largeInput);
    
    EXPECT_NO_THROW(tm.Run());
    EXPECT_EQ(tm.GetTape().Read(100000), '0');
}

// Проверка движения головки в обе стороны (вправо и влево)
TEST(TuringMachineTest, MoveLeftAndRight) {
    TuringMachine tm("q0", "halt", '_');
    
    tm.AddRule(TuringRule("q0", '1', '2', MoveDirection::Right, "q1"));
    tm.AddRule(TuringRule("q1", '0', '1', MoveDirection::Left, "q2"));
    tm.AddRule(TuringRule("q2", '2', '3', MoveDirection::None, "halt"));

    tm.LoadInput("10"); // [1][0]
    tm.Run();

    // [3][1]
    EXPECT_EQ(tm.GetTape().Read(0), '3');
    EXPECT_EQ(tm.GetTape().Read(1), '1');
    EXPECT_EQ(tm.GetCurrentState(), "halt");
}

//Пустая входная лента
TEST(TuringMachineTest, EmptyInputTape) {
    TuringMachine tm("start", "halt", '_');
    
    tm.AddRule(TuringRule("start", '_', 'X', MoveDirection::None, "halt"));
    
    tm.LoadInput("");
    tm.Run();
    
    EXPECT_EQ(tm.GetTape().Read(0), 'X');
    EXPECT_EQ(tm.GetCurrentState(), "halt");
}

//Отсутствие правила
TEST(TuringMachineTest, MissingRuleThrowsOrHalts) {
    TuringMachine tm("start", "halt", '_');
    
    tm.AddRule(TuringRule("start", '1', '2', MoveDirection::Right, "start"));
    tm.LoadInput("0");

    EXPECT_THROW(tm.Run(), std::runtime_error); 
}

TEST(TuringMachineTest, UnaryAdditionAlgorithm) {
    TuringMachine tm("q_find_plus", "halt", '_');
    
    tm.AddRule(TuringRule("q_find_plus", '1', '1', MoveDirection::Right, "q_find_plus"));
    tm.AddRule(TuringRule("q_find_plus", '+', '1', MoveDirection::Right, "q_go_to_end"));
    tm.AddRule(TuringRule("q_go_to_end", '1', '1', MoveDirection::Right, "q_go_to_end"));
    tm.AddRule(TuringRule("q_go_to_end", '_', '_', MoveDirection::Left, "q_erase_last"));
    tm.AddRule(TuringRule("q_erase_last", '1', '_', MoveDirection::None, "halt"));

    // 2 + 3
    tm.LoadInput("11+111"); 
    tm.Run();

    // Ожидаемый результат: "11111_" = 5
    EXPECT_EQ(tm.GetTape().Read(0), '1');
    EXPECT_EQ(tm.GetTape().Read(1), '1');
    EXPECT_EQ(tm.GetTape().Read(2), '1');
    EXPECT_EQ(tm.GetTape().Read(3), '1');
    EXPECT_EQ(tm.GetTape().Read(4), '1');
    
    EXPECT_EQ(tm.GetTape().Read(5), '_'); 

    EXPECT_EQ(tm.GetCurrentState(), "halt");
}