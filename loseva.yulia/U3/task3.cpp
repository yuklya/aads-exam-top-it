#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgsS3(const int argc, char** argv, std::string& inFile, Array_t< std::string >& dataFiles)
{
  bool inFound = false;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg.find("data:") == 0) {
      loseva::push_back(dataFiles, arg.substr(5));
    } else if (arg.find("in:") == 0) {
      if (inFound) {
        return false;
      }
      inFile = arg.substr(3);
      inFound = true;
    } else {
      return false;
    }
  }
  return (dataFiles.size > 0);
}

void loadMeetsS3(const std::string& file, Array_t< MeetWithDate_t >& meets)
{
  std::ifstream fin(file);
  if (!fin.is_open()) {
    std::cerr << "Open fail\n";
    std::exit(2);
  }
  int d = 0;
  int m = 0;
  int y = 0;
  std::size_t id1 = 0;
  std::size_t id2 = 0;
  int time = 0;
  while (fin >> d >> m >> y >> id1 >> id2 >> time) {
    if (id1 == id2) {
      continue;
    }
    loseva::MeetWithDate_t mwd;
    mwd.date.d = d;
    mwd.date.m = m;
    mwd.date.y = y;
    mwd.meet.id1 = id1;
    mwd.meet.id2 = id2;
    mwd.meet.time = time;
    loseva::push_back(meets, mwd);
  }
  if (fin.fail() && !fin.eof()) {
    std::cerr << "Format error\n";
    std::exit(3);
  }
}

void calculateInitialPeriod(const Array_t< MeetWithDate_t >& meets, Period_t& prd)
{
  if (meets.size == 0) {
    prd.is_empty = true;
    return;
  }
  prd.is_empty = false;
  prd.start = meets.data[0].date;
  prd.end = meets.data[0].date;
  for (std::size_t i = 1; i < meets.size; ++i) {
    if (loseva::compare_dates(meets.data[i].date, prd.start) < 0) {
      prd.start = meets.data[i].date;
    }
    if (loseva::compare_dates(meets.data[i].date, prd.end) > 0) {
      prd.end = meets.data[i].date;
    }
  }
}

void handleRange(const Period_t& prd)
{
  if (prd.is_empty) {
    std::cout << "<EMPTY>\n";
  } else {
    std::cout << prd.start.d << " " << prd.start.m << " " << prd.start.y << " "
              << prd.end.d << " " << prd.end.m << " " << prd.end.y << "\n";
  }
}

}

int main(const int argc, char** argv)
{
  std::string inFile = "";
  loseva::Array_t< std::string > dataFiles;
  loseva::init(dataFiles);
  if (!loseva::parseArgsS3(argc, argv, inFile, dataFiles)) {
    std::cerr << "Invalid args\n";
    loseva::destroy(dataFiles);
    return 1;
  }
  loseva::Array_t< loseva::Person_t > persons;
  loseva::init(persons);
  loseva::Array_t< loseva::MeetWithDate_t > meets;
  loseva::init(meets);
  for (std::size_t i = 0; i < dataFiles.size; ++i) {
    loseva::loadMeetsS3(dataFiles.data[i], meets);
  }
  loseva::Period_t current_period;
  loseva::calculateInitialPeriod(meets, current_period);
  loseva::RangeStack_t history;
  loseva::init(history);
  std::string cmd = "";
  while (std::cin >> cmd) {
    if (cmd == "range") {
      loseva::handleRange(current_period);
    } else if (cmd == "before") {
      int d = 0;
      int m = 0;
      int y = 0;
      if (std::cin >> d >> m >> y) {
        if (current_period.is_empty) {
          std::cout << "<INVALID COMMAND>\n";
        } else {
          loseva::Date_t limit;
          limit.d = d;
          limit.m = m;
          limit.y = y;
          loseva::push(history, current_period);
          if (loseva::compare_dates(current_period.start, limit) > 0) {
            current_period.is_empty = true;
          } else if (loseva::compare_dates(current_period.end, limit) > 0) {
            current_period.end = limit;
          }
        }
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
    } else if (cmd == "after") {
      int d = 0;
      int m = 0;
      int y = 0;
      if (std::cin >> d >> m >> y) {
        if (current_period.is_empty) {
          std::cout << "<INVALID COMMAND>\n";
        } else {
          loseva::Date_t limit;
          limit.d = d;
          limit.m = m;
          limit.y = y;
          loseva::push(history, current_period);
          if (loseva::compare_dates(current_period.end, limit) < 0) {
            current_period.is_empty = true;
          } else if (loseva::compare_dates(current_period.start, limit) < 0) {
            current_period.start = limit;
          }
        }
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
    } else if (cmd == "pop-range") {
      if (history.size == 0) {
        std::cout << "<INVALID COMMAND>\n";
      } else {
        current_period = history.data[history.size - 1];
        loseva::pop(history);
      }
    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
    std::string dummy = "";
    std::getline(std::cin, dummy);
  }
  loseva::destroy(persons);
  loseva::destroy(meets);
  loseva::destroy(dataFiles);
  loseva::destroy(history);
  return 0;
}
