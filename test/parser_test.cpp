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

  parser.parseMain();
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

  parser.parseMain();
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

  parser.parseMain();
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

  parser.parseMain();
  ASSERT_EQ(parser.dump(), "x:\n(a ^ 2)\n");
}
TEST(Test, ParserComplexArith) {
  Parser parser({"x", ",", "y", "=", "a", "+", "b", ",", "a", "-", "b"},
                {"a", "b"});
  parser.parseMain();
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
  parser.parseMain();
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
  parser.parseMain();
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
  parser.parseMain();
  parser.genRev();
  ASSERT_EQ(parser.dumpRev(), "\nb : \nx\nx\n\na : \nx\nx\n");
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
  parser.parseMain();
  parser.genRev();
  ASSERT_EQ(parser.dumpRev(),
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
  parser.parseMain();
  parser.genRev();
  ASSERT_EQ(parser.dumpRev(),
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
  parser.parseMain();
  parser.genRev();
  ASSERT_EQ(parser.dumpRev(), "\nb : \nx\n(0 - x)\n\na : \nx\n(0 - x)\n");
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
  parser.parseMain();
  parser.genRev();

  ASSERT_EQ(parser.dumpRev(), "\na : \n(2 * (x * a))\n");
}
