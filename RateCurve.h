#ifndef RATE_CURVE_H
#define RATE_CURVE_H

#include "Date.h"
#include <map>
#include <string>

// Zero-rate curve (continuous compounding). Rates are stored in percent, as
// supplied by the market data, and returned in percent by getRate(); decimals
// are used internally for discounting.
class RateCurve {
public:
  RateCurve() = default;
  RateCurve(const std::string &name, const Date &asOf)
      : name(name), asOf(asOf) {}

  void addRate(const std::string &tenor, double ratePct);

  // Rate (in percent) at an exact tenor label, with linear interpolation in
  // time across the pillars and flat extrapolation at the ends.
  double getRate(const std::string &tenor) const;
  double getRateAtYears(double years) const; // percent

  // Discount factor df = exp(-r * t), r in decimal, t = ACT/365 to _date.
  double getDf(const Date &_date) const;
  double getDfAtYears(double years) const;

  // Return a copy with every pillar shifted by shockPct (percent), e.g. +0.01
  // for a +1bp parallel bump. Used for DV01.
  RateCurve bumped(double shockPct) const;

  void display() const;

  std::string name;
  Date asOf;

private:
  // tenor years -> rate in percent, kept sorted for interpolation.
  std::map<double, double> rates;
};

#endif // RATE_CURVE_H
