#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "computer_club.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  std::string input_file_name = argv[1];
  std::ifstream input_file(input_file_name);

  if (!input_file.is_open()) {
    std::cerr << "Error: Could not open file " << input_file_name << std::endl;
    return 1;
  }

  ComputerClub club;
  std::optional<std::string> config_error_line =
      club.loadConfiguration(input_file);

  if (config_error_line.has_value()) {
    std::cout << config_error_line.value() << std::endl;
    input_file.close();
    return 0;
  }

  std::cout << club.getOpenTime().toString() << std::endl;

  std::string event_line_str;
  Time last_event_time(0, 0);
  bool first_event = true;

  while (std::getline(input_file, event_line_str)) {
    if (event_line_str.empty()) {
      continue;
    }
    std::string time_str_from_event;
    std::istringstream iss_event_peek(event_line_str);
    iss_event_peek >> time_str_from_event;

    Time current_event_time;
    try {
      current_event_time = Time::parse(time_str_from_event);
    } catch (const std::runtime_error &) {
      // Ошибка формата времени в строке события
      std::cout << event_line_str << std::endl;
      input_file.close();
      return 0;
    }

    if (!first_event && current_event_time < last_event_time) {
      // Нарушение последовательности времени событий
      std::cout << event_line_str << std::endl;
      input_file.close();
      return 0;
    }

    std::optional<std::string> event_format_error =
        club.processEventLine(event_line_str);

    if (event_format_error.has_value()) {
      std::cout << event_format_error.value() << std::endl;
      input_file.close();
      return 0;
    }

    last_event_time = current_event_time;
    first_event = false;
  }

  club.processEndOfDay();

  for (const auto &logged_event : club.getEventLog()) {
    std::cout << logged_event.toString() << std::endl;
  }

  std::cout << club.getCloseTime().toString() << std::endl;

  for (const auto &table_stat_line : club.getTableStatistics()) {
    std::cout << table_stat_line << std::endl;
  }

  input_file.close();
  return 0;
}