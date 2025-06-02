#include "computer_club.h"
#include <iostream>
#include <limits>

namespace utils {
int parsePositiveInteger(const std::string &s) {
  if (s.empty()) {
    return -1;
  }
  for (char c : s) {
    if (!std::isdigit(c)) {
      return -1;
    }
  }
  try {
    if (s.length() > 1 && s[0] == '0') {
      return -1;
    }
    unsigned long long val = std::stoull(s);
    if (val == 0 || val > static_cast<unsigned long long>(
                              std::numeric_limits<int>::max())) {
      return -1;
    }
    return static_cast<int>(val);
  } catch (const std::out_of_range &) {
    return -1;
  } catch (const std::invalid_argument &) {
    return -1;
  }
}

bool isValidIntegerString(const std::string &s, int &out_val, int min_val,
                          int max_val) {
  if (s.empty())
    return false;
  for (char c : s) {
    if (!std::isdigit(c))
      return false;
  }
  try {
    if (s.length() > 1 && s[0] == '0')
      return false;
    if (s == "0" && min_val > 0)
      return false;

    long long val = std::stoll(s);
    if (val < min_val || val > max_val) {
      return false;
    }
    out_val = static_cast<int>(val);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

bool isValidClientName(const std::string &name) {
  if (name.empty())
    return false;
  for (char c : name) {
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' ||
          c == '-')) {
      return false;
    }
  }
  return true;
}
} // namespace utils

// --- struct Time ---
Time::Time() : hours(0), minutes(0) {}

Time::Time(int h, int m) : hours(h), minutes(m) {}

Time::Time(int total_minutes) {
  if (total_minutes < 0) {
    hours = 0;
    minutes = 0;
  } else {
    hours = total_minutes / 60;
    minutes = total_minutes % 60;
  }
}

int Time::toMinutes() const { return hours * 60 + minutes; }

std::string Time::toString() const {
  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
      << std::setfill('0') << minutes;
  return oss.str();
}

bool Time::operator<(const Time &other) const {
  return toMinutes() < other.toMinutes();
}
bool Time::operator<=(const Time &other) const {
  return toMinutes() <= other.toMinutes();
}
bool Time::operator>(const Time &other) const {
  return toMinutes() > other.toMinutes();
}
bool Time::operator>=(const Time &other) const {
  return toMinutes() >= other.toMinutes();
}
bool Time::operator==(const Time &other) const {
  return toMinutes() == other.toMinutes();
}
bool Time::operator!=(const Time &other) const {
  return toMinutes() != other.toMinutes();
}

int Time::minutesUntil(const Time &futureTime) const {
  return futureTime.toMinutes() - this->toMinutes();
}

Time Time::addMinutes(int mins_to_add) const {
  return Time(this->toMinutes() + mins_to_add);
}

Time Time::parse(const std::string &s) {
  if (s.length() != 5 || s[2] != ':') {
    throw std::runtime_error("Invalid time format: " + s);
  }
  try {
    int h = std::stoi(s.substr(0, 2));
    int m = std::stoi(s.substr(3, 2));
    if (h < 0 || h > 23 || m < 0 || m > 59) {
      throw std::runtime_error("Invalid time value: " + s);
    }
    return Time(h, m);
  } catch (const std::invalid_argument &) {
    throw std::runtime_error("Time string part is not a number: " + s);
  } catch (const std::out_of_range &) {
    throw std::runtime_error("Time string number out of range: " + s);
  }
}

// --- struct Event ---
Event::Event(const Time &t, int ev_id, const std::string &c_name, int tbl_id,
             const std::string &err_msg)
    : event_time(t), event_id(ev_id), client_name(c_name), table_id_val(tbl_id),
      error_message(err_msg) {}

Event Event::newClientEvent(const Time &t, int id, std::string cName) {
  return Event(t, id, std::move(cName), 0, "");
}

Event Event::newClientTableEvent(const Time &t, int id, std::string cName,
                                 int tbl_id) {
  return Event(t, id, std::move(cName), tbl_id, "");
}

Event Event::newErrorEvent(const Time &t, std::string errMsg) {
  return Event(t, 13, "", 0, std::move(errMsg));
}

std::string Event::toString() const {
  std::ostringstream oss;
  oss << event_time.toString() << " " << event_id;

  if (event_id == 13) {
    oss << " " << error_message;
  } else if (!client_name.empty() && table_id_val != 0) {
    oss << " " << client_name << " " << table_id_val;
  } else if (!client_name.empty()) {
    oss << " " << client_name;
  }
  return oss.str();
}

// --- struct TableInfo ---
TableInfo::TableInfo(int table_id) : id(table_id) {}

void TableInfo::occupy(const std::string &client_name,
                       const Time &current_time) {
  is_occupied = true;
  current_client_name = client_name;
  session_start_time = current_time;
}

void TableInfo::free(const Time &current_time, int hour_price) {
  if (!is_occupied)
    return;

  int duration_minutes = session_start_time.minutesUntil(current_time);

  if (duration_minutes < 0) {
    duration_minutes = 0;
  }

  total_minutes_used += duration_minutes;

  int billed_hours = (duration_minutes + 59) / 60;
  revenue_generated += billed_hours * hour_price;

  is_occupied = false;
  current_client_name = "";
}

// --- struct ClientInfo ---
ClientInfo::ClientInfo(ClientLocation loc, int tbl_id)
    : location(loc), table_id(tbl_id) {}

// --- class ComputerClub ---
ComputerClub::ComputerClub() {}

void ComputerClub::addEventToLog(const Event &event) {
  event_log_output.push_back(event);
}

void ComputerClub::addErrorEventToLog(const Time &event_time,
                                      const std::string &error_message) {
  Event err_event = Event::newErrorEvent(event_time, error_message);
  event_log_output.push_back(err_event);
}

bool ComputerClub::isClientInClub(const std::string &client_name) const {
  return clients_in_club_state.count(client_name);
}

bool ComputerClub::isWorkingTime(const Time &current_time) const {
  return current_time >= open_time_config && current_time < close_time_config;
}

int ComputerClub::findFreeTable() const {
  for (const auto &table : tables_state) {
    if (!table.is_occupied) {
      return table.id;
    }
  }
  return 0;
}

std::optional<std::string>
ComputerClub::loadConfiguration(std::istream &configFileStream) {
  std::string line;
  // 1. Count tables
  if (!std::getline(configFileStream, line))
    return "";

  std::string num_tables_str;
  std::istringstream iss_tables(line);
  iss_tables >> num_tables_str;
  if (iss_tables.fail()) {
    return line;
  }
  std::string temp_extra_tables;
  if (iss_tables >> temp_extra_tables) {
    return line;
  } // extra data

  this->num_tables_config = utils::parsePositiveInteger(num_tables_str);
  if (this->num_tables_config == -1)
    return line;

  // 2. working hours
  if (!std::getline(configFileStream, line))
    return "";

  std::string open_time_str, close_time_str;
  std::istringstream iss_times(line);
  iss_times >> open_time_str >> close_time_str;

  if (iss_times.fail())
    return line;
  std::string temp_extra_times;
  if (iss_times >> temp_extra_times)
    return line; // extra data

  try {
    this->open_time_config = Time::parse(open_time_str);
    this->close_time_config = Time::parse(close_time_str);
  } catch (const std::runtime_error &) {
    return line;
  }

  if (!(this->open_time_config < this->close_time_config))
    return line;

  // 3. cost of hour
  if (!std::getline(configFileStream, line))
    return "";

  std::string hourly_rate_str;
  std::istringstream iss_rate(line);
  iss_rate >> hourly_rate_str;
  if (iss_rate.fail()) {
    return line;
  }
  std::string temp_extra_rate;
  if (iss_rate >> temp_extra_rate) {
    return line;
  } // extra data

  this->hourly_rate_config = utils::parsePositiveInteger(hourly_rate_str);
  if (this->hourly_rate_config == -1)
    return line;

  this->tables_state.clear();
  this->tables_state.reserve(this->num_tables_config);
  for (int i = 0; i < this->num_tables_config; ++i) {
    this->tables_state.emplace_back(i + 1);
  }
  return std::nullopt;
}

std::optional<ComputerClub::ParsedEventInput>
ComputerClub::parseEventDetails(const std::string &eventLine) {
  std::istringstream iss(eventLine);
  std::string time_str, id_str;
  ParsedEventInput data;

  iss >> time_str >> id_str;
  if (iss.fail())
    return std::nullopt;

  try {
    data.time = Time::parse(time_str);
  } catch (const std::runtime_error &) {
    return std::nullopt;
  }

  if (!utils::isValidIntegerString(id_str, data.id, 1, 4)) {
    return std::nullopt;
  }

  std::string client_name_str_temp;
  std::string table_id_str_param_temp;
  int table_id_param_val = 0;

  switch (data.id) {
  case 1:
  case 3:
  case 4:
    iss >> client_name_str_temp;
    if (iss.fail() || !utils::isValidClientName(client_name_str_temp))
      return std::nullopt;
    // extra data
    if (iss >> table_id_str_param_temp)
      return std::nullopt;
    data.client_name = client_name_str_temp;
    data.table_id = 0;
    break;
  case 2: {
    iss >> client_name_str_temp >> table_id_str_param_temp;
    if (iss.fail() || !utils::isValidClientName(client_name_str_temp))
      return std::nullopt;
    if (!utils::isValidIntegerString(table_id_str_param_temp,
                                     table_id_param_val, 1,
                                     this->num_tables_config))
      return std::nullopt;

    // extra data
    std::string temp_extra_event2;
    if (iss >> temp_extra_event2)
      return std::nullopt;
    data.client_name = client_name_str_temp;
    data.table_id = table_id_param_val;
  } break;
  default:
    return std::nullopt;
  }
  return data;
}

void ComputerClub::handleClientArrived(const Time &event_time,
                                       const std::string &client_name) {
  if (this->isClientInClub(client_name)) {
    this->addErrorEventToLog(event_time, "YouShallNotPass");
  } else if (!this->isWorkingTime(event_time)) {
    this->addErrorEventToLog(event_time, "NotOpenYet");
  } else {
    clients_in_club_state[client_name] =
        ClientInfo(ClientLocation::INSIDE_CLUB_NOT_AT_TABLE, 0);
  }
}

void ComputerClub::handleClientSat(const Time &event_time,
                                   const std::string &client_name,
                                   int table_id) {
  if (!this->isClientInClub(client_name)) {
    this->addErrorEventToLog(event_time, "ClientUnknown");
  } else if (tables_state[table_id - 1].is_occupied) {
    this->addErrorEventToLog(event_time, "PlaceIsBusy");
  } else {
    ClientInfo &clientInfo = clients_in_club_state[client_name];

    if (clientInfo.table_id != 0 && clientInfo.table_id != table_id) {
      tables_state[clientInfo.table_id - 1].free(event_time,
                                                 hourly_rate_config);
    } else if (clientInfo.table_id == table_id) { // PlaceIsBusy
    }

    tables_state[table_id - 1].occupy(client_name, event_time);
    clientInfo.location = ClientLocation::AT_TABLE;
    clientInfo.table_id = table_id;
  }
}

void ComputerClub::handleClientWaited(const Time &event_time,
                                      const std::string &client_name) {
  if (!this->isClientInClub(client_name)) {
    this->addErrorEventToLog(event_time, "ClientUnknown");
    return;
  }

  ClientInfo &clientInfo = clients_in_club_state[client_name];

  // 1: have free table
  if (this->findFreeTable() != 0 &&
      clientInfo.location != ClientLocation::AT_TABLE) {
    this->addErrorEventToLog(event_time, "ICanWaitNoLonger!");
    return;
  }

  // 2: queue is full
  if (waiting_queue_state.size() >= static_cast<size_t>(num_tables_config) &&
      clientInfo.location != ClientLocation::AT_TABLE) {
    this->addEventToLog(Event::newClientEvent(event_time, 11, client_name));
    clients_in_club_state.erase(client_name);
    return;
  }

  if (clientInfo.location == ClientLocation::AT_TABLE) {
    if (waiting_queue_state.size() >= static_cast<size_t>(num_tables_config)) {
      return;
    }

    int current_table_id = clientInfo.table_id;

    tables_state[current_table_id - 1].free(event_time, hourly_rate_config);
    clientInfo.table_id = 0;
    clientInfo.location = ClientLocation::IN_QUEUE;

    waiting_queue_state.push_back(client_name);

    if (!waiting_queue_state.empty()) {
      std::string first_in_queue = waiting_queue_state.front();

      if (first_in_queue != client_name || waiting_queue_state.size() > 1) {
        if (first_in_queue != client_name) {
          waiting_queue_state.pop_front();

          if (clients_in_club_state.count(first_in_queue)) {
            ClientInfo &occupant_info = clients_in_club_state[first_in_queue];
            tables_state[current_table_id - 1].occupy(first_in_queue,
                                                      event_time);
            occupant_info.location = ClientLocation::AT_TABLE;
            occupant_info.table_id = current_table_id;
            this->addEventToLog(Event::newClientTableEvent(
                event_time, 12, first_in_queue, current_table_id));
          }
        } else {
        }
      }
    }

  } else if (clientInfo.location == ClientLocation::IN_QUEUE) {
  } else {
    bool already_in_queue = false;
    for (const auto &name_in_q : waiting_queue_state) {
      if (name_in_q == client_name) {
        already_in_queue = true;
        break;
      }
    }
    if (!already_in_queue) {
      waiting_queue_state.push_back(client_name);
    }
    clientInfo.location = ClientLocation::IN_QUEUE;
  }
}

void ComputerClub::handleClientLeft(const Time &event_time,
                                    const std::string &client_name) {
  if (!this->isClientInClub(client_name)) {
    this->addErrorEventToLog(event_time, "ClientUnknown");
  } else {
    ClientInfo client_original_info = clients_in_club_state[client_name];
    clients_in_club_state.erase(client_name);

    if (client_original_info.table_id != 0) {
      int freed_table_id = client_original_info.table_id;
      tables_state[freed_table_id - 1].free(event_time, hourly_rate_config);

      if (!waiting_queue_state.empty()) {
        std::string next_client_name_from_queue = waiting_queue_state.front();
        waiting_queue_state.pop_front();

        if (clients_in_club_state.count(next_client_name_from_queue)) {
          ClientInfo &next_client_info_ref =
              clients_in_club_state[next_client_name_from_queue];

          tables_state[freed_table_id - 1].occupy(next_client_name_from_queue,
                                                  event_time);
          next_client_info_ref.location = ClientLocation::AT_TABLE;
          next_client_info_ref.table_id = freed_table_id;
          this->addEventToLog(Event::newClientTableEvent(
              event_time, 12, next_client_name_from_queue, freed_table_id));
        } else {
        }
      }
    } else if (client_original_info.location == ClientLocation::IN_QUEUE) {
      auto &queue = waiting_queue_state;
      auto it = std::find(queue.begin(), queue.end(), client_name);
      if (it != queue.end()) {
        queue.erase(it);
      }
    }
  }
}

std::optional<std::string>
ComputerClub::processEventLine(const std::string &eventLine) {

  std::optional<ParsedEventInput> parsed_data = parseEventDetails(eventLine);

  if (!parsed_data.has_value()) {
    return eventLine;
  }

  const auto &data = parsed_data.value();
  const Time &event_time = data.time;
  int event_id_val = data.id;
  const std::string &client_name_str = data.client_name;
  int table_id_param = data.table_id;

  if (event_id_val == 2) {
    this->addEventToLog(Event::newClientTableEvent(
        event_time, event_id_val, client_name_str, table_id_param));
  } else {
    this->addEventToLog(
        Event::newClientEvent(event_time, event_id_val, client_name_str));
  }

  switch (event_id_val) {
  case 1:
    handleClientArrived(event_time, client_name_str);
    break;
  case 2:
    handleClientSat(event_time, client_name_str, table_id_param);
    break;
  case 3:
    handleClientWaited(event_time, client_name_str);
    break;
  case 4:
    handleClientLeft(event_time, client_name_str);
    break;
  }
  return std::nullopt;
}

void ComputerClub::processEndOfDay() {
  std::vector<std::string> remaining_clients_names;
  for (const auto &pair : clients_in_club_state) {
    remaining_clients_names.push_back(pair.first);
  }
  std::sort(remaining_clients_names.begin(), remaining_clients_names.end());

  for (const std::string &client_name : remaining_clients_names) {
    ClientInfo clientInfo = clients_in_club_state[client_name];
    if (clientInfo.table_id != 0) {
      tables_state[clientInfo.table_id - 1].free(this->close_time_config,
                                                 this->hourly_rate_config);
    }
    this->addEventToLog(
        Event::newClientEvent(this->close_time_config, 11, client_name));
  }
  clients_in_club_state.clear();
  waiting_queue_state.clear();
}

const Time &ComputerClub::getOpenTime() const { return open_time_config; }
const Time &ComputerClub::getCloseTime() const { return close_time_config; }
const std::vector<Event> &ComputerClub::getEventLog() const {
  return event_log_output;
}

std::vector<std::string> ComputerClub::getTableStatistics() const {
  std::vector<std::string> stats;
  stats.reserve(this->num_tables_config);
  for (const auto &table : this->tables_state) {
    Time duration_occupied(table.total_minutes_used);
    std::ostringstream oss;
    oss << table.id << " " << table.revenue_generated << " "
        << duration_occupied.toString();
    stats.push_back(oss.str());
  }
  return stats;
}