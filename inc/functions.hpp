#ifndef FUNCTIONS
#define FUNCTIONS

#include <bitset>
#include <iostream>
#include <sstream>
#include <fstream>

std::string str_to_bin (const std::string&);
std::string str_to_bin (const char*, size_t);
std::string hex_to_bin (const std::string&);
std::ostream& hex_to_stream (std::ostream&, const std::string&);
std::string bin_to_str (const std::bitset<512>&);
std::string read (std::istream&, size_t&);
std::string& flip_str (std::string &);

#endif
