#pragma once

#include <algorithm>
#include <deque>
#include <iomanip>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// --- struct for time ---
struct Time {
  int hours;
  int minutes;

  Time();
  Time(int h, int m);
  explicit Time(int total_minutes);

  int toMinutes() const;
  std::string toString() const;

  bool operator<(const Time &other) const;
  bool operator<=(const Time &other) const;
  bool operator>(const Time &other) const;
  bool operator>=(const Time &other) const;
  bool operator==(const Time &other) const;
  bool operator!=(const Time &other) const;

  int minutesUntil(const Time &futureTime) const;
  Time addMinutes(int mins_to_add) const;
  static Time parse(const std::string &s);
};

// --- struct for event ---
struct Event {
public:
  Time event_time;
  int event_id;
  std::string client_name;
  int table_id_val = 0;
  std::string error_message;

private:
  Event(const Time &t, int ev_id, const std::string &c_name, int tbl_id,
        const std::string &err_msg);

public:
  Event() = default;

  static Event newClientEvent(const Time &t, int id, std::string cName);
  static Event newClientTableEvent(const Time &t, int id, std::string cName,
                                   int tbl_id);
  static Event newErrorEvent(const Time &t, std::string errMsg);

  std::string toString() const;
};

// --- informatuion about table ---
struct TableInfo {
  int id;
  bool is_occupied = false;
  std::string current_client_name = "";
  Time session_start_time;
  int total_minutes_used = 0;
  int revenue_generated = 0;

  TableInfo(int table_id = 0);

  void occupy(const std::string &client_name, const Time &current_time);
  void free(const Time &current_time, int hour_price);
};

enum class ClientLocation { INSIDE_CLUB_NOT_AT_TABLE, AT_TABLE, IN_QUEUE };

// --- information avout client in club ---
struct ClientInfo {
  ClientLocation location;
  int table_id = 0;

  ClientInfo(ClientLocation loc = ClientLocation::INSIDE_CLUB_NOT_AT_TABLE,
             int tblId = 0);
};

// --- main class computer club ---
class ComputerClub {
private:
  int num_tables_config = 0;
  Time open_time_config;
  Time close_time_config;
  int hourly_rate_config = 0;

  std::vector<TableInfo> tables_state;
  std::map<std::string, ClientInfo> clients_in_club_state;
  std::deque<std::string> waiting_queue_state;

  std::vector<Event> event_log_output;

  struct ParsedEventInput {
    Time time;
    int id;
    std::string client_name;
    int table_id = 0;
  };
  std::optional<ParsedEventInput>
  parseEventDetails(const std::string &eventLine);

  void handleClientArrived(const Time &event_time,
                           const std::string &client_name);
  void handleClientSat(const Time &event_time, const std::string &client_name,
                       int table_id);
  void handleClientWaited(const Time &event_time,
                          const std::string &client_name);
  void handleClientLeft(const Time &event_time, const std::string &client_name);

  void addEventToLog(const Event &event);
  void addErrorEventToLog(const Time &event_time,
                          const std::string &error_message);
  bool isClientInClub(const std::string &client_name) const;
  bool isWorkingTime(const Time &current_time) const;
  int findFreeTable() const;

public:
  ComputerClub();

  std::optional<std::string> loadConfiguration(std::istream &configFileStream);
  std::optional<std::string> processEventLine(const std::string &eventLine);
  void processEndOfDay();

  const Time &getOpenTime() const;
  const Time &getCloseTime() const;
  const std::vector<Event> &getEventLog() const;
  std::vector<std::string> getTableStatistics() const;
};

bool isValidClientName(const std::string &name);