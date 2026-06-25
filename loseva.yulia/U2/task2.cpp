#include <iostream>
#include <fstream>
#include <string>
#include "shared_ds.hpp"

namespace loseva {

bool parseArgsS2(const int argc, char** argv, std::string& inFile, std::string& dataFile)
{
  bool dataFound = false;
  bool inFound = false;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg.find("data:") == 0) {
      if (dataFound) {
        return false;
      }
      dataFile = arg.substr(5);
      dataFound = true;
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
  return dataFound;
}

void ensure_person(Array< Person >& persons, const std::size_t id)
{
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == id) {
      return;
    }
  }
  Person p;
  p.id = id;
  p.info = "";
  p.has_description = false;
  p.is_removed = false;
  loseva::push_back(persons, p);
}

bool person_exists(const Array< Person >& persons, const std::size_t id)
{
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == id && !persons.data[i].is_removed) {
      return true;
    }
  }
  return false;
}

void loadPersonsS2(const std::string& file, Array< Person >& persons)
{
  std::ifstream fin(file);
  if (!fin.is_open()) {
    std::cerr << "Open fail\n";
    std::exit(2);
  }
  std::string line = "";
  while (std::getline(fin, line)) {
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
      continue;
    }
    std::size_t id = 0;
    for (std::size_t i = 0; i < id_str.size(); ++i) {
      id = id * 10 + (id_str[i] - '0');
    }
    const std::size_t info_start = info_str.find_first_not_of(" \t");
    if (info_start == std::string::npos) {
      continue;
    }
    info_str = info_str.substr(info_start);
    bool dup = false;
    for (std::size_t i = 0; i < persons.size; ++i) {
      if (persons.data[i].id == id) {
        dup = true;
        break;
      }
    }
    if (dup) {
      continue;
    }
    Person p;
    p.id = id;
    p.info = info_str;
    p.has_description = true;
    p.is_removed = false;
    loseva::push_back(persons, p);
  }
}

void loadMeets(const std::string& file, Array< Meet >& meets, Array< Person >& persons)
{
  std::ifstream fin(file);
  if (!fin.is_open()) {
    std::cerr << "Open fail\n";
    std::exit(2);
  }
  std::size_t id1 = 0;
  std::size_t id2 = 0;
  int time = 0;
  while (fin >> id1 >> id2 >> time) {
    if (id1 == id2) {
      continue;
    }
    Meet m;
    m.id1 = id1;
    m.id2 = id2;
    m.time = time;
    loseva::push_back(meets, m);
    loseva::ensure_person(persons, id1);
    loseva::ensure_person(persons, id2);
  }
  if (fin.fail() && !fin.eof()) {
    std::cerr << "Format error\n";
    std::exit(3);
  }
}

void handleAnons(const Array< Person >& persons)
{
  Array< std::size_t > anon_ids;
  loseva::init(anon_ids);
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (!persons.data[i].has_description && !persons.data[i].is_removed) {
      loseva::push_back(anon_ids, persons.data[i].id);
    }
  }
  for (std::size_t i = 0; i < anon_ids.size; ++i) {
    for (std::size_t j = i + 1; j < anon_ids.size; ++j) {
      if (anon_ids.data[i] > anon_ids.data[j]) {
        const std::size_t tmp = anon_ids.data[i];
        anon_ids.data[i] = anon_ids.data[j];
        anon_ids.data[j] = tmp;
      }
    }
  }
  for (std::size_t i = 0; i < anon_ids.size; ++i) {
    std::cout << anon_ids.data[i] << "\n";
  }
  loseva::destroy(anon_ids);
}

void handleDeanon(Array< Person >& persons, Array< Meet >& meets, const std::size_t anon_id, const std::size_t id)
{
  int anon_idx = -1;
  int id_idx = -1;
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == anon_id && !persons.data[i].is_removed) {
      anon_idx = static_cast< int >(i);
    }
    if (persons.data[i].id == id && !persons.data[i].is_removed) {
      id_idx = static_cast< int >(i);
    }
  }
  if (anon_idx == -1 || id_idx == -1 || persons.data[anon_idx].has_description || !persons.data[id_idx].has_description) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  persons.data[anon_idx].is_removed = true;
  for (std::size_t i = 0; i < meets.size; ++i) {
    if (meets.data[i].id1 == anon_id) {
      meets.data[i].id1 = id;
    }
    if (meets.data[i].id2 == anon_id) {
      meets.data[i].id2 = id;
    }
  }
  Array< Meet > filtered;
  loseva::init(filtered);
  for (std::size_t i = 0; i < meets.size; ++i) {
    if (meets.data[i].id1 != meets.data[i].id2) {
      loseva::push_back(filtered, meets.data[i]);
    }
  }
  delete[] meets.data;
  meets = filtered;
}

void handleDesc(const Array< Person >& persons, const std::size_t id)
{
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == id && !persons.data[i].is_removed) {
      if (persons.data[i].has_description) {
        std::cout << persons.data[i].info << "\n";
      } else {
        std::cout << "<ANON>\n";
      }
      return;
    }
  }
  std::cout << "<INVALID COMMAND>\n";
}

void handleRedesc(Array< Person >& persons, const std::size_t id, const std::string& info)
{
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].id == id && !persons.data[i].is_removed) {
      persons.data[i].info = info;
      persons.data[i].has_description = true;
      return;
    }
  }
  std::cout << "<INVALID COMMAND>\n";
}

void handleMeets(const Array< Person >& persons, const Array< Meet >& meets, const std::size_t id)
{
  if (!loseva::person_exists(persons, id)) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  Array< MeetPair > pairs;
  loseva::init(pairs);
  for (std::size_t i = 0; i < meets.size; ++i) {
    if (meets.data[i].id1 == id) {
      MeetPair mp;
      mp.other_id = meets.data[i].id2;
      mp.time = meets.data[i].time;
      loseva::push_back(pairs, mp);
    } else if (meets.data[i].id2 == id) {
      MeetPair mp;
      mp.other_id = meets.data[i].id1;
      mp.time = meets.data[i].time;
      loseva::push_back(pairs, mp);
    }
  }
  for (std::size_t i = 0; i < pairs.size; ++i) {
    for (std::size_t j = i + 1; j < pairs.size; ++j) {
      if (pairs.data[i].other_id > pairs.data[j].other_id ||
          (pairs.data[i].other_id == pairs.data[j].other_id && pairs.data[i].time > pairs.data[j].time)) {
        const MeetPair tmp = pairs.data[i];
        pairs.data[i] = pairs.data[j];
        pairs.data[j] = tmp;
      }
    }
  }
  for (std::size_t i = 0; i < pairs.size; ++i) {
    std::cout << pairs.data[i].other_id << " " << pairs.data[i].time << "\n";
  }
  loseva::destroy(pairs);
}

void handleLessGreater(const Array< Person >& persons, const Array< Meet >& meets, const std::size_t id, const int time, const bool is_less)
{
  if (!loseva::person_exists(persons, id)) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  Array< MeetPair > pairs;
  loseva::init(pairs);
  for (std::size_t i = 0; i < meets.size; ++i) {
    bool match = false;
    std::size_t other = 0;
    if (meets.data[i].id1 == id) {
      match = true;
      other = meets.data[i].id2;
    } else if (meets.data[i].id2 == id) {
      match = true;
      other = meets.data[i].id1;
    }
    if (match) {
      if ((is_less && meets.data[i].time < time) || (!is_less && meets.data[i].time > time)) {
        MeetPair mp;
        mp.other_id = other;
        mp.time = meets.data[i].time;
        loseva::push_back(pairs, mp);
      }
    }
  }
  for (std::size_t i = 0; i < pairs.size; ++i) {
    for (std::size_t j = i + 1; j < pairs.size; ++j) {
      if (pairs.data[i].other_id > pairs.data[j].other_id ||
          (pairs.data[i].other_id == pairs.data[j].other_id && pairs.data[i].time > pairs.data[j].time)) {
        const MeetPair tmp = pairs.data[i];
        pairs.data[i] = pairs.data[j];
        pairs.data[j] = tmp;
      }
    }
  }
  for (std::size_t i = 0; i < pairs.size; ++i) {
    std::cout << pairs.data[i].other_id << " " << pairs.data[i].time << "\n";
  }
  loseva::destroy(pairs);
}

void handleCommons(const Array< Person >& persons, const Array< Meet >& meets, const std::size_t id1, const std::size_t id2)
{
  if (!loseva::person_exists(persons, id1) || !loseva::person_exists(persons, id2)) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  Array< std::size_t > p1_partners;
  loseva::init(p1_partners);
  Array< std::size_t > p2_partners;
  loseva::init(p2_partners);
  for (std::size_t i = 0; i < meets.size; ++i) {
    if (meets.data[i].id1 == id1) {
      loseva::push_back(p1_partners, meets.data[i].id2);
    } else if (meets.data[i].id2 == id1) {
      loseva::push_back(p1_partners, meets.data[i].id1);
    }
    if (meets.data[i].id1 == id2) {
      loseva::push_back(p2_partners, meets.data[i].id2);
    } else if (meets.data[i].id2 == id2) {
      loseva::push_back(p2_partners, meets.data[i].id1);
    }
  }
  Array< std::size_t > common;
  loseva::init(common);
  for (std::size_t i = 0; i < p1_partners.size; ++i) {
    const std::size_t partner = p1_partners.data[i];
    bool found_in_p2 = false;
    for (std::size_t j = 0; j < p2_partners.size; ++j) {
      if (p2_partners.data[j] == partner) {
        found_in_p2 = true;
        break;
      }
    }
    if (found_in_p2) {
      bool already_in_common = false;
      for (std::size_t k = 0; k < common.size; ++k) {
        if (common.data[k] == partner) {
          already_in_common = true;
          break;
        }
      }
      if (!already_in_common) {
        loseva::push_back(common, partner);
      }
    }
  }
  for (std::size_t i = 0; i < common.size; ++i) {
    for (std::size_t j = i + 1; j < common.size; ++j) {
      if (common.data[i] > common.data[j]) {
        const std::size_t tmp = common.data[i];
        common.data[i] = common.data[j];
        common.data[j] = tmp;
      }
    }
  }
  for (std::size_t i = 0; i < common.size; ++i) {
    std::cout << common.data[i] << "\n";
  }
  if (common.size == 0) {
    std::cout << "\n";
  }
  loseva::destroy(p1_partners);
  loseva::destroy(p2_partners);
  loseva::destroy(common);
}

void handleOutPersons(const Array< Person >& persons, const std::string& filename)
{
  std::ofstream fout(filename);
  if (!fout.is_open()) {
    return;
  }
  for (std::size_t i = 0; i < persons.size; ++i) {
    if (persons.data[i].has_description && !persons.data[i].is_removed) {
      fout << persons.data[i].id << " " << persons.data[i].info << "\n";
    }
  }
}

std::string read_quoted_string()
{
  char ch = ' ';
  while (std::cin.get(ch) && ch != '"') {
    if (ch == '\n') {
      return "";
    }
  }
  std::string result = "";
  while (std::cin.get(ch) && ch != '"') {
    result += ch;
  }
  return result;
}

void skip_to_end_of_line()
{
  char ch = ' ';
  while (std::cin.get(ch) && ch != '\n');
}

}

int main(const int argc, char** argv)
{
  std::string inFile = "";
  std::string dataFile = "";
  if (!loseva::parseArgsS2(argc, argv, inFile, dataFile) || argc < 2 || argc > 3) {
    std::cerr << "Invalid args\n";
    return 1;
  }
  loseva::Array< loseva::Person > persons;
  loseva::init(persons);
  loseva::Array< loseva::Meet > meets;
  loseva::init(meets);
  if (!inFile.empty()) {
    loseva::loadPersonsS2(inFile, persons);
  }
  loseva::loadMeets(dataFile, meets, persons);
  std::string cmd = "";
  while (std::cin >> cmd) {
    if (cmd == "anons") {
      loseva::handleAnons(persons);
      loseva::skip_to_end_of_line();
    } else if (cmd == "deanon") {
      std::size_t anon_id = 0;
      std::size_t id = 0;
      if (std::cin >> anon_id >> id) {
        loseva::handleDeanon(persons, meets, anon_id, id);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "desc") {
      std::size_t id = 0;
      if (std::cin >> id) {
        char peek_ch = ' ';
        while (std::cin.peek() == ' ' || std::cin.peek() == '\t') {
          std::cin.get(peek_ch);
        }
        if (std::cin.peek() == '"') {
          const std::string info = loseva::read_quoted_string();
          loseva::handleRedesc(persons, id, info);
        } else {
          loseva::handleDesc(persons, id);
        }
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "redesc") {
      std::size_t id = 0;
      if (std::cin >> id) {
        const std::string info = loseva::read_quoted_string();
        loseva::handleRedesc(persons, id, info);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "meet" || cmd == "meets") {
      std::size_t id = 0;
      if (std::cin >> id) {
        loseva::handleMeets(persons, meets, id);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "commons") {
      std::size_t id1 = 0;
      std::size_t id2 = 0;
      if (std::cin >> id1 >> id2) {
        loseva::handleCommons(persons, meets, id1, id2);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "less") {
      int time = 0;
      std::size_t id = 0;
      if (std::cin >> time >> id) {
        loseva::handleLessGreater(persons, meets, id, time, true);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "greater") {
      int time = 0;
      std::size_t id = 0;
      if (std::cin >> time >> id) {
        loseva::handleLessGreater(persons, meets, id, time, false);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else if (cmd == "out-persons") {
      std::string filename = "";
      if (std::cin >> filename) {
        loseva::handleOutPersons(persons, filename);
      } else {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
      }
      loseva::skip_to_end_of_line();
    } else {
      std::cout << "<INVALID COMMAND>\n";
      loseva::skip_to_end_of_line();
    }
  }
  loseva::destroy(persons);
  loseva::destroy(meets);
  return 0;
}
