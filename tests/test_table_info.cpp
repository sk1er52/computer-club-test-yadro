#include "computer_club.h"
#include "gtest/gtest.h"

class TableInfoTest : public ::testing::Test {
protected:
  TableInfo table;
  const int hourly_rate = 10;

  TableInfoTest() : table(1) {}

  void SetUp() override {
    table = TableInfo(1);
    table.is_occupied = false;
    table.current_client_name = "";
    table.total_minutes_used = 0;
    table.revenue_generated = 0;
  }
};

TEST_F(TableInfoTest, InitialState) {
  ASSERT_EQ(table.id, 1);
  ASSERT_FALSE(table.is_occupied);
  ASSERT_TRUE(table.current_client_name.empty());
  ASSERT_EQ(table.total_minutes_used, 0);
  ASSERT_EQ(table.revenue_generated, 0);
}

TEST_F(TableInfoTest, OccupyTable) {
  Time occupy_time(10, 0);
  table.occupy("client1", occupy_time);

  ASSERT_TRUE(table.is_occupied);
  ASSERT_EQ(table.current_client_name, "client1");
  ASSERT_EQ(table.session_start_time.toString(), "10:00");
}

TEST_F(TableInfoTest, FreeTableNotOccupied) {
  Time free_time(11, 0);
  table.free(free_time, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 0);
  ASSERT_EQ(table.revenue_generated, 0);
}

TEST_F(TableInfoTest, FreeTableLessThanHour) {
  Time occupy_time(10, 0);
  Time free_time(10, 30);
  table.occupy("client1", occupy_time);
  table.free(free_time, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 30);
  ASSERT_EQ(table.revenue_generated, 1 * hourly_rate);
}

TEST_F(TableInfoTest, FreeTableExactlyOneHour) {
  Time occupy_time(10, 0);
  Time free_time(11, 0);
  table.occupy("client1", occupy_time);
  table.free(free_time, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 60);
  ASSERT_EQ(table.revenue_generated, 1 * hourly_rate);
}
TEST_F(TableInfoTest, FreeTableMoreThanHourButLessThanTwo) {
  Time occupy_time(10, 0);
  Time free_time(11, 15);
  table.occupy("client1", occupy_time);
  table.free(free_time, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 75);
  ASSERT_EQ(table.revenue_generated, 2 * hourly_rate);
}

TEST_F(TableInfoTest, FreeTableMultipleSessions) {
  // session 1: 30 minutes
  table.occupy("client1", Time(10, 0));
  table.free(Time(10, 30), hourly_rate);
  ASSERT_EQ(table.total_minutes_used, 30);
  ASSERT_EQ(table.revenue_generated, 1 * hourly_rate);

  // session 2: 75 minutes
  table.occupy("client2", Time(11, 0));
  table.free(Time(12, 15), hourly_rate);
  ASSERT_EQ(table.total_minutes_used, 30 + 75);
  ASSERT_EQ(table.revenue_generated, (1 * hourly_rate) + (2 * hourly_rate));
  ASSERT_FALSE(table.is_occupied);
}

TEST_F(TableInfoTest, FreeTableZeroDuration) {
  Time time_now(10, 0);
  table.occupy("client1", time_now);
  table.free(time_now, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 0);
  ASSERT_EQ(table.revenue_generated, 0 * hourly_rate);
}

TEST_F(TableInfoTest, FreeTableNegativeDurationSafeguard) {
  Time occupy_time(10, 0);
  Time free_time_error(9, 0);
  table.occupy("client1", occupy_time);
  table.free(free_time_error, hourly_rate);

  ASSERT_FALSE(table.is_occupied);
  ASSERT_EQ(table.total_minutes_used, 0);
  ASSERT_EQ(table.revenue_generated, 0);
}