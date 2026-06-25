#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgs(const int argc, char** argv, std::string& inFile, std::string& outFile)
{
  bool inFound = false;
  bool outFound = false;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg.find("in:") == 0) {
      if (inFound) {
        return false;
      }
      inFile = arg.substr(3);
      inFound = true;
    } else if (arg.find("out:") == 0) {
      if (outFound) {
        return false;
      }
      outFile = arg.substr(4);
      outFound = true;
    } else {
      return false;
    }
  }
  return true;
}

}

int main(const int argc, char** argv)
{
  std::string inFile = "";
  std::string outFile = "";
  if (!loseva::parseArgs(argc, argv, inFile, outFile) || argc > 3) {
    std::cerr << "Invalid arguments\n";
    return 1;
  }
  std::istream* in = &std::cin;
  std::ostream* out = &std::cout;
  std::ifstream fin;
  std::ofstream fout;
  if (!inFile.empty()) {
    fin.open(inFile);
    if (!fin.is_open()) {
      return 2;
    }
    in = &fin;
  }
  loseva::Array< loseva::Person > persons;
  loseva::init(persons);
  std::size_t successCount = 0;
  std::size_t ignoredCount = 0;
  std::string line = "";
  while (std::getline(*in, line)) {
    const std::size_t first_non_space = line.find_first_not_of(" \t\r\n");
    if (first_non_space == std::string::npos) {
      continue;
    }
    const std::size_t id_end = line.find_first_of(" \t", first_non_space);
    std::string id_str = "";
    std::string info_str = "";
    if (id_end == std::string::npos) {
      id_str = line.substr(first_non_space);
    } else {
      id_str = line.substr(first_non_space, id_end - first_non_space);
      info_str = line.substr(id_end);
    }
    bool is_num = true;
    if (id_str.empty()) {
      is_num = false;
    } else {
      for (std::size_t i = 0; i < id_str.size(); ++i) {
        if (id_str[i] < '0' || id_str[i] > '9') {
          is_num = false;
          break;
        }
      }
    }
    if (!is_num) {
      ignoredCount++;
      continue;
    }
    std::size_t id = 0;
    for (std::size_t i = 0; i < id_str.size(); ++i) {
      id = id * 10 + (id_str[i] - '0');
    }
    const std::size_t info_start = info_str.find_first_not_of(" \t");
    if (info_start == std::string::npos) {
      ignoredCount++;
      continue;
    }
    info_str = info_str.substr(info_start);
    if (loseva::has_person(persons, id)) {
      ignoredCount++;
      continue;
    }
    loseva::Person p;
    p.id = id;
    p.info = info_str;
    loseva::push_back(persons, p);
    successCount++;
  }
  if (!outFile.empty()) {
    fout.open(outFile);
    if (!fout.is_open()) {
      loseva::destroy(persons);
      return 2;
    }
    out = &fout;
  }
  for (std::size_t i = 0; i < persons.size; ++i) {
    *out << persons.data[i].id << " " << persons.data[i].info << "\n";
  }
  std::cerr << successCount << " " << ignoredCount << "\n";
  loseva::destroy(persons);
  return 0;
}
