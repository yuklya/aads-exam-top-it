#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgs(int argc, char** argv, std::string& inFile, std::string& outFile) {
  if (argc > 3) return false;
  bool inFound = false, outFound = false;
  
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("in:") == 0) {
      if (inFound) return false;
      inFile = arg.substr(3);
      inFound = true;
    } else if (arg.find("out:") == 0) {
      if (outFound) return false;
      outFile = arg.substr(4);
      outFound = true;
    } else {
      return false;
    }
  }
  return true;
}

}

int main(int argc, char** argv) {
  std::string inFile = "", outFile = "";
  if (!loseva::parseArgs(argc, argv, inFile, outFile)) return 1;
  
  std::istream* in = &std::cin;
  std::ostream* out = &std::cout;
  std::ifstream fin;
  std::ofstream fout;
  
  if (!inFile.empty()) {
    fin.open(inFile);
    if (!fin.is_open()) return 2;
    in = &fin;
  }
  
  loseva::Array<loseva::Person> persons;
  loseva::init(persons);
  
  std::size_t successCount = 0, ignoredCount = 0, id;
  while (*in >> id) {
    std::string info;
    std::getline(*in, info);
    
    std::size_t start = info.find_first_not_of(" \t");
    info = (start != std::string::npos) ? info.substr(start) : "";
    
    if (info.empty() || loseva::has_person(persons, id)) {
      ignoredCount++;
    } else {
      loseva::Person p;
      p.id = id; p.info = info;
      loseva::push_back(persons, p);
      successCount++;
    }
  }
  
  if (in->fail() && !in->eof()) {
    in->clear();
    std::string dummy;
    std::getline(*in, dummy);
    ignoredCount++;
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
