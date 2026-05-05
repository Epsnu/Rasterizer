#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

std::vector<std::string> split_by_whitespace(const std::string& input);

#endif