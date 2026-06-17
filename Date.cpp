#include "Date.h"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>

// days_from_civil (Howard Hinnant's algorithm): exact serial day number.
long Date::serial() const {
  int y = year;
  const unsigned m = static_cast<unsigned>(month);
  const unsigned d = static_cast<unsigned>(day);
  y -= m <= 2;
  const long era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097L + static_cast<long>(doe) - 719468L;
}

Date Date::fromString(const std::string &s) {
  if (s.empty() || s == "null") {
    return Date();
  }
  int y = 0, m = 0, d = 0;
  char dash1 = 0, dash2 = 0;
  std::istringstream ss(s);
  ss >> y >> dash1 >> m >> dash2 >> d;
  if (ss.fail() || dash1 != '-' || dash2 != '-') {
    return Date();
  }
  return Date(y, m, d);
}

std::string Date::toString() const {
  std::ostringstream os;
  os << year << "-" << std::setw(2) << std::setfill('0') << month << "-"
     << std::setw(2) << std::setfill('0') << day;
  return os.str();
}

double operator-(const Date &d1, const Date &d2) {
  return static_cast<double>(d1.serial() - d2.serial()) / 365.0;
}

bool operator>(const Date &d1, const Date &d2) {
  return d1.serial() > d2.serial();
}

bool operator<(const Date &d1, const Date &d2) {
  return d1.serial() < d2.serial();
}

bool operator==(const Date &d1, const Date &d2) {
  return d1.serial() == d2.serial();
}

std::ostream &operator<<(std::ostream &os, const Date &d) {
  os << d.toString();
  return os;
}

std::istream &operator>>(std::istream &is, Date &d) {
  char sep = 0;
  is >> d.year >> sep >> d.month >> sep >> d.day;
  return is;
}

double tenorToYears(const std::string &tenor) {
  if (tenor == "ON" || tenor == "on") {
    return 1.0 / 365.0;
  }
  if (tenor.empty()) {
    return 0.0;
  }
  const char unit = static_cast<char>(std::toupper(tenor.back()));
  const double n = std::atof(tenor.substr(0, tenor.size() - 1).c_str());
  switch (unit) {
  case 'D':
    return n / 365.0;
  case 'W':
    return n * 7.0 / 365.0;
  case 'M':
    return n / 12.0;
  case 'Y':
    return n;
  default:
    return 0.0;
  }
}

Date dateAddTenor(const Date &start, const std::string &tenorStr) {
  const double years = tenorToYears(tenorStr);
  const int totalMonths = start.year * 12 + (start.month - 1) +
                          static_cast<int>(years * 12.0 + 0.5);
  int y = totalMonths / 12;
  int m = totalMonths % 12 + 1;
  return Date(y, m, start.day);
}
