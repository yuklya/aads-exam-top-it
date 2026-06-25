#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgsT3(int argc, char** argv, std::string& inFile, Array<std::string>& dataFiles) {
  bool inFound = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("data:") == 0) {
      push_back(dataFiles, arg.substr(5));
    } else if (arg.find("in:") == 0) {
      if (inFound) return false;
      inFile = arg.substr(3);
      inFound = true;
    } else {
      return false;
    }
  }
  return (dataFiles.size > 0);
}

void loadMeetsT3(const std::string& file, Array<MeetWithDate>& meets) {
  std::ifstream fin(file);
  if (!fin.is_open()) {
    std::cerr << "Open fail\n";
    std::exit(2);
  }
  int d, m, y;
  std::size_t id1, id2;
  int time;
  while (fin >> d >> m >> y >> id1 >> id2 >> time) {
    if (id1 == id2) continue;
    MeetWithDate mwd;
    mwd.date.d = d; mwd.date.m = m; mwd.date.y = y;
    mwd.meet.id1 = id1; mwd.meet.id2 = id2; mwd.meet.time = time;
    push_back(meets, mwd);
  }
  if (fin.fail() && !fin.eof()) {
    std::cerr << "Format error\n";
    std::exit(3);
  }
}

void calculateInitialPeriod(const Array<MeetWithDate>& meets, Period& prd) {
  if (meets.size == 0) {
    prd.is_empty = true;
    return;
  }
  prd.is_empty = false;
  prd.start = meets.data[0].date;
  prd.end = meets.data[0].date;
  for (std::size_t i = 1; i < meets.size; ++i) {
    if (compare_dates(meets.data[i].date, prd.start) < 0) prd.start = meets.data[i].date;
    if (compare_dates(meets.data[i].date, prd.end) > 0) prd.end = meets.data[i].date;
  }
}

bool dateInPeriod(const Date& dt, const Period& prd) {
  if (prd.is_empty) return false;
  return (compare_dates(dt, prd.start) >= 0 && compare_dates(dt, prd.end) <= 0);
}

void handleRange(const Period& prd) {
  if (prd.is_empty) {
    std::cout << "<EMPTY>\n";
  } else {
    std::cout << prd.start.d << " " << prd.start.m << " " << prd.start.y << " "
              << prd.end.d << " " << prd.end.m << " " << prd.end.y << "\n";
  }
}

}

int main(int argc, char** argv) {
  std::string inFile = "";
  loseva::Array<std::string> dataFiles;
  loseva::init(dataFiles);
  
  if (!loseva::parseArgsT3(argc, argv, inFile, dataFiles)) {
    std::cerr << "Invalid args\n";
    loseva::destroy(dataFiles);
    return 1;
  }
  
  loseva::Array<loseva::Person> persons;
  loseva::init(persons);
  loseva::Array<loseva::MeetWithDate> meets;
  loseva::init(meets);
  
  for (std::size_t i = 0; i < dataFiles.size; ++i) {
    loseva::loadMeetsT3(dataFiles.data[i], meets);
  }
  
  loseva::Period current_period;
  loseva::calculateInitialPeriod(meets, current_period);
  
  loseva::RangeStack history;
  loseva::init(history);
  
  std::string cmd;
  while (std::cin >> cmd) {
    if (cmd == "range") {
      loseva::handleRange(current_period);
    } else if (cmd == "before") {
      int d, m, y;
      if (std::cin >> d >> m >> y) {
        if (current_period.is_empty) {
          std::cout << "<INVALID COMMAND>\n";
        } else {
          loseva::Date limit; limit.d = d; limit.m = m; limit.y = y;
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
      int d, m, y;
      if (std::cin >> d >> m >> y) {
        if (current_period.is_empty) {
          std::cout << "<INVALID COMMAND>\n";
        } else {
          loseva::Date limit; limit.d = d; limit.m = m; limit.y = y;
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
    std::string dummy;
    std::getline(std::cin, dummy);
  }
  
  loseva::destroy(persons);
  loseva::destroy(meets);
  loseva::destroy(dataFiles);
  loseva::destroy(history);
  return 0;
}
