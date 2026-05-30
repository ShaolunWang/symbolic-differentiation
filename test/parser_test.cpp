#include "parser.h"
#include "gtest/gtest.h"
TEST(Test, PlusMinusBrackets) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "+",
          "(",
          "b",
          "-",
          "c",
          ")",
      },
      {"a", "b", "c"});

  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\n(a + (b - c))\n");
}
TEST(Test, PlusMinus) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "+",
          "b",
          "-",
          "c",
      },
      {"a", "b", "c"});

  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\n((a + b) - c)\n");
}
TEST(Test, Variable) {
  Parser parser(
      {
          "x",
          "=",
          "a",
      },
      {"a"});

  parser.parse_main();
  ;
  ASSERT_EQ(parser.dump(), "x:\na\n");
}

TEST(Test, UnaryOperator) {

  Parser parser(
      {
          "x",
          "=",
          "a",
          "^",
          "2",
      },
      {"a"});

  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\n(a ^ 2)\n");
}
TEST(Test, ParserComplexArith) {
  Parser parser({"x", ",", "y", "=", "a", "+", "b", ",", "a", "-", "b"},
                {"a", "b"});
  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\n(a + b)\ny:\n(a - b)\n");
}

TEST(Test, ParserSimpleBracket) {

  Parser parser(
      {
          "x",
          "=",
          "(",
          "a",
          ")",
      },
      {"a"});
  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\na\n");
}

TEST(Test, ParserHierarchy) {

  Parser parser(
      {
          "x",
          "=",
          "a",
          "+",
          "b",
          "*",
          "c",
      },
      {"a", "b", "c"});
  parser.parse_main();
  ASSERT_EQ(parser.dump(), "x:\n(a + (b * c))\n");
}
TEST(Test, RevBasicAdd) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "+",
          "b",
      },
      {"a", "b"});
  parser.parse_main();
  parser.gen_rev();
  ASSERT_EQ(parser.dump_rev(), "\nb : \nx\nx\n\na : \nx\nx\n");
}
TEST(Test, RevBasicMul) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "*",
          "b",
      },
      {"a", "b"});
  parser.parse_main();
  parser.gen_rev();
  ASSERT_EQ(parser.dump_rev(),
            "\nb : \n(x * b)\n(x * a)\n\na : \n(x * b)\n(x * a)\n");
}
TEST(Test, RevBasicDiv) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "/",
          "b",
      },
      {"a", "b"});
  parser.parse_main();
  parser.gen_rev();
  ASSERT_EQ(parser.dump_rev(),
            "\nb : \n(x / b)\n(0 - (x * (a / (b ^ 2))))\n\na : \n(x / b)\n(0 - "
            "(x * (a / (b ^ 2))))\n");
}
TEST(Test, RevBasicSub) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "-",
          "b",
      },
      {"a", "b"});
  parser.parse_main();
  parser.gen_rev();
  ASSERT_EQ(parser.dump_rev(), "\nb : \nx\n(0 - x)\n\na : \nx\n(0 - x)\n");
}
TEST(Test, RevBasicSq) {
  Parser parser(
      {
          "x",
          "=",
          "a",
          "^",
          "2",
      },
      {"a"});
  parser.parse_main();
  parser.gen_rev();

  ASSERT_EQ(parser.dump_rev(), "\na : \n(2 * (x * a))\n");
}
