#include "computer_club.h"
#include "gtest/gtest.h"

TEST(EventTest, CreateClientEvent) {
  Time t(10, 30);
  Event e = Event::newClientEvent(t, 1, "client_alpha");
  ASSERT_EQ(e.event_time.toString(), "10:30");
  ASSERT_EQ(e.event_id, 1);
  ASSERT_EQ(e.client_name, "client_alpha");
  ASSERT_EQ(e.table_id_val, 0);
  ASSERT_TRUE(e.error_message.empty());
}

TEST(EventTest, CreateClientTableEvent) {
  Time t(11, 00);
  Event e = Event::newClientTableEvent(t, 2, "client_beta", 5);
  ASSERT_EQ(e.event_time.toString(), "11:00");
  ASSERT_EQ(e.event_id, 2);
  ASSERT_EQ(e.client_name, "client_beta");
  ASSERT_EQ(e.table_id_val, 5);
  ASSERT_TRUE(e.error_message.empty());
}

TEST(EventTest, CreateErrorEvent) {
  Time t(12, 15);
  Event e = Event::newErrorEvent(t, "TestError");
  ASSERT_EQ(e.event_time.toString(), "12:15");
  ASSERT_EQ(e.event_id, 13);
  ASSERT_TRUE(e.client_name.empty());
  ASSERT_EQ(e.table_id_val, 0);
  ASSERT_EQ(e.error_message, "TestError");
}

TEST(EventTest, ToStringClientEvent) {
  Time t(9, 5);
  Event e = Event::newClientEvent(t, 1, "client1");
  ASSERT_EQ(e.toString(), "09:05 1 client1");

  Event e_leave = Event::newClientEvent(t, 4, "long-client-name_123");
  ASSERT_EQ(e_leave.toString(), "09:05 4 long-client-name_123");
}

TEST(EventTest, ToStringClientTableEvent) {
  Time t(14, 20);
  Event e = Event::newClientTableEvent(t, 2, "client2", 3);
  ASSERT_EQ(e.toString(), "14:20 2 client2 3");

  Event e_sit_from_queue =
      Event::newClientTableEvent(t, 12, "client_gamma", 10);
  ASSERT_EQ(e_sit_from_queue.toString(), "14:20 12 client_gamma 10");
}

TEST(EventTest, ToStringErrorEvent) {
  Time t(16, 0);
  Event e = Event::newErrorEvent(t, "PlaceIsBusy");
  ASSERT_EQ(e.toString(), "16:00 13 PlaceIsBusy");
}