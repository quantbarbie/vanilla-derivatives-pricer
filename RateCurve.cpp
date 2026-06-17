#include "RateCurve.h"

#include <cmath>
#include <iostream>

void RateCurve::addRate(const std::string &tenor, double ratePct) {
  rates[tenorToYears(tenor)] = ratePct;
}

double RateCurve::getRateAtYears(double years) const {
  if (rates.empty()) {
    return 0.0;
  }
  // Flat extrapolation beyond the first / last pillar.
  if (years <= rates.begin()->first) {
    return rates.begin()->second;
  }
  if (years >= rates.rbegin()->first) {
    return rates.rbegin()->second;
  }
  auto hi = rates.lower_bound(years); // first pillar >= years
  auto lo = std::prev(hi);
  const double t0 = lo->first, r0 = lo->second;
  const double t1 = hi->first, r1 = hi->second;
  const double w = (years - t0) / (t1 - t0);
  return r0 + w * (r1 - r0);
}

double RateCurve::getRate(const std::string &tenor) const {
  return getRateAtYears(tenorToYears(tenor));
}

double RateCurve::getDfAtYears(double years) const {
  if (years <= 0.0) {
    return 1.0;
  }
  const double r = getRateAtYears(years) / 100.0; // percent -> decimal
  return std::exp(-r * years);
}

double RateCurve::getDf(const Date &_date) const {
  return getDfAtYears(_date - asOf);
}

RateCurve RateCurve::bumped(double shockPct) const {
  RateCurve out = *this;
  for (auto &kv : out.rates) {
    kv.second += shockPct;
  }
  return out;
}

void RateCurve::display() const {
  std::cout << "Rate Curve: " << name << " as of " << asOf << std::endl;
  for (const auto &kv : rates) {
    std::cout << "  " << kv.first << "Y: " << kv.second << "%" << std::endl;
  }
}
