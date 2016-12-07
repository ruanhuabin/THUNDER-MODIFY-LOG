#ifndef UTILS_H
#define UTILS_H

#include <boost/container/vector.hpp>
#include <string>

using std::string;
using boost::container::vector;

bool regexMatches(const char* str, const char* pattern);

#endif // UTILS_H
