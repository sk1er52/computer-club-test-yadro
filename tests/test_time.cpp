#include "computer_club.h"
#include "gtest/gtest.h"

TEST(TimeTest, ParseValidTime) {
  Time t = Time::parse("08:05");
  ASSERT_EQ(t.hours, 8);
  ASSERT_EQ(t.minutes, 5);

  t = Time::parse("23:59");
  ASSERT_EQ(t.hours, 23);
  ASSERT_EQ(t.minutes, 59);

  t = Time::parse("00:00");
  ASSERT_EQ(t.hours, 0);
  ASSERT_EQ(t.minutes, 0);
}

TEST(TimeTest, ParseInvalidFormat) {
  EXPECT_THROW(Time::parse("8:05"), std::runtime_error);
  EXPECT_THROW(Time::parse("08:5"), std::runtime_error);
  EXPECT_THROW(Time::parse("0805"), std::runtime_error);
  EXPECT_THROW(Time::parse("08:055"), std::runtime_error);
  EXPECT_THROW(Time::parse("aa:bb"), std::runtime_error);
}

TEST(TimeTest, ParseInvalidValues) {
  EXPECT_THROW(Time::parse("24:00"), std::runtime_error);
  EXPECT_THROW(Time::parse("00:60"), std::runtime_error);
  EXPECT_THROW(Time::parse("-1:00"), std::runtime_error);
}

TEST(TimeTest, ToString) {
  Time t1(8, 5);
  ASSERT_EQ(t1.toString(), "08:05");
  Time t2(12, 30);
  ASSERT_EQ(t2.toString(), "12:30");
  Time t3(0, 0);
  ASSERT_EQ(t3.toString(), "00:00");
}

TEST(TimeTest, Comparison) {
  Time t1 = Time::parse("10:00");
  Time t2 = Time::parse("12:00");
  Time t3 = Time::parse("10:00");

  ASSERT_TRUE(t1 < t2);
  ASSERT_FALSE(t2 < t1);
  ASSERT_TRUE(t1 <= t2);
  ASSERT_TRUE(t1 <= t3);
  ASSERT_TRUE(t1 == t3);
  ASSERT_FALSE(t1 == t2);
  ASSERT_TRUE(t1 != t2);
  ASSERT_FALSE(t1 != t3);
}

TEST(TimeTest, ToMinutes) {
  Time t(10, 30);
  ASSERT_EQ(t.toMinutes(), 10 * 60 + 30);
  Time t_zero(0, 0);
  ASSERT_EQ(t_zero.toMinutes(), 0);
}

TEST(TimeTest, AddMinutes) {
  Time t1 = Time::parse("10:30");
  Time t2 = t1.addMinutes(90);
  ASSERT_EQ(t2.toString(), "12:00");

  Time t3 = Time::parse("23:30");
  Time t4 = t3.addMinutes(60);
  ASSERT_EQ(t4.hours, 24);
  ASSERT_EQ(t4.minutes, 30);
}

TEST(TimeTest, MinutesUntil) {
  Time start_time = Time::parse("09:00");
  Time end_time = Time::parse("10:30");
  ASSERT_EQ(start_time.minutesUntil(end_time), 90);
  ASSERT_EQ(end_time.minutesUntil(start_time), -90);
}