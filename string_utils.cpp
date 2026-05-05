#include <algorithm>
#include <sstream>

#include "string_utils.h"

// Trim from the start (in place)
void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
};

// Trim from the end (in place)
void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
};

// Trim from both ends (in place)
void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
};

std::vector<std::string> split_by_whitespace(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> result;
    std::string word;
    while (iss >> word) {
        result.push_back(word);
    }
    return result;
};