#include "computer_club.h"
#include "gtest/gtest.h"

TEST(ParserUtilsTest, IsValidClientName) {
  ASSERT_TRUE(utils::isValidClientName("client1"));
  ASSERT_TRUE(utils::isValidClientName("client-0_name"));
  ASSERT_TRUE(utils::isValidClientName("a"));
  ASSERT_TRUE(utils::isValidClientName("0"));
  ASSERT_TRUE(utils::isValidClientName("_"));
  ASSERT_TRUE(utils::isValidClientName("-"));

  ASSERT_FALSE(utils::isValidClientName(""));
  ASSERT_FALSE(utils::isValidClientName("client!name"));
  ASSERT_FALSE(utils::isValidClientName("client name"));
  ASSERT_FALSE(utils::isValidClientName("ClientName"));
}

TEST(ParserUtilsTest, ParsePositiveInteger) {
  ASSERT_EQ(utils::parsePositiveInteger("1"), 1);
  ASSERT_EQ(utils::parsePositiveInteger("123"), 123);
  ASSERT_EQ(utils::parsePositiveInteger("2147483647"), 2147483647);

  ASSERT_EQ(utils::parsePositiveInteger("0"), -1);
  ASSERT_EQ(utils::parsePositiveInteger("-1"), -1);
  ASSERT_EQ(utils::parsePositiveInteger("abc"), -1);
  ASSERT_EQ(utils::parsePositiveInteger(""), -1);
  ASSERT_EQ(utils::parsePositiveInteger("1.0"), -1);
  ASSERT_EQ(utils::parsePositiveInteger("1 2"), -1);
  ASSERT_EQ(utils::parsePositiveInteger(" 123"), -1);
  ASSERT_EQ(utils::parsePositiveInteger("123 "), -1);
  ASSERT_EQ(utils::parsePositiveInteger("01"), -1);
  ASSERT_EQ(utils::parsePositiveInteger("2147483648"), -1);
}

TEST(ParserUtilsTest, IsValidIntegerStringInRange) {
  int value;
  ASSERT_TRUE(utils::isValidIntegerString("10", value, 5, 15));
  ASSERT_EQ(value, 10);
  ASSERT_TRUE(utils::isValidIntegerString("5", value, 5, 15));
  ASSERT_EQ(value, 5);
  ASSERT_TRUE(utils::isValidIntegerString("15", value, 5, 15));
  ASSERT_EQ(value, 15);
  ASSERT_TRUE(utils::isValidIntegerString("0", value, 0, 10));
  ASSERT_EQ(value, 0);

  ASSERT_FALSE(utils::isValidIntegerString("4", value, 5, 15));
  ASSERT_FALSE(utils::isValidIntegerString("16", value, 5, 15));
  ASSERT_FALSE(utils::isValidIntegerString("-1", value, 0, 10));

  ASSERT_FALSE(utils::isValidIntegerString("abc", value, 0, 10));
  ASSERT_FALSE(utils::isValidIntegerString("", value, 0, 10));
  ASSERT_FALSE(utils::isValidIntegerString("1.0", value, 0, 10));
  ASSERT_FALSE(utils::isValidIntegerString("1 0", value, 0, 10));
  ASSERT_FALSE(utils::isValidIntegerString("01", value, 0, 10));
  ASSERT_TRUE(utils::isValidIntegerString("0", value, 0, 10));

  ASSERT_TRUE(utils::isValidIntegerString("0", value, 0, 0));
  ASSERT_EQ(value, 0);
  ASSERT_FALSE(utils::isValidIntegerString("00", value, 0, 0));

  ASSERT_TRUE(utils::isValidIntegerString("2000000000", value, 1, 2147483647));
  ASSERT_EQ(value, 2000000000);
  ASSERT_FALSE(utils::isValidIntegerString("3000000000", value, 1, 2147483647));
}