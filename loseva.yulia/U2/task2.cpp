#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgsT2(int argc, char** argv, std::string& inFile, std::string& dataFile) {
  if (argc < 2 || argc > 3) return false;
  bool dataFound = false, inFound = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("data:") == 0) {
      if (dataFound) return false;
      dataFile = arg.substr(5);
      dataFound = true;
    } else if (arg.find("in:") == 0) {
      if (inFound) return false;
      inFile = arg.substr(3);
      inFound = true;
    } else {
      return false;
    }
  }
  return dataFound;
}

void loadMeets(const std::string& file, Array<Meet>& meets) {
  std::ifstream fin(file);
  if (!fin.is_open()) {
    std::cerr << "Open fail\n";
    std::exit(2);
  }
  std::size_t id1, id2;
  int time;
  while (fin >> id1 >> id2 >> time) {
    if (id1 == id2) continue;
    Meet m; m.id1 = id1; m.id2 = id2; m.time = time;
    push_back(meets, m);
  }
  if (fin.fail() && !fin.eof()) {
    std::cerr << "Format error\n";
    std::exit(3);
  }
}

void handleDesc(const Array<Person>& persons, std::size_t id) {
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == id) {
      std::cout << persons.data[i].info << "\n";
      return;
    }
  }
  std::cout << "<ANON>\n";
}

}

int main(int argc, char** argv) {
  std::string inFile = "", dataFile = "";
  if (!loseva::parseArgsT2(argc, argv, inFile, dataFile)) {
    std::cerr << "Invalid args\n";
    return 1;
  }
  
  loseva::Array<loseva::Person> persons;
  loseva::init(persons);
  loseva::Array<loseva::Meet> meets;
  loseva::init(meets);
  
  loseva::loadMeets(dataFile, meets);
  
  std::string cmd;
  while (std::cin >> cmd) {
    if (cmd == "desc") {
      std::size_t id;
      if (std::cin >> id) {
        loseva::handleDesc(persons, id);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
    std::string dummy;
    std::getline(std::cin, dummy);
  }
  
  loseva::destroy(persons);
  loseva::destroy(meets);
  return 0;
}
