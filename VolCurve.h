#ifndef VOL_CURVE_H
#define VOL_CURVE_H

#include "Date.h"
#include <map>
#include <string>

// ATM volatility curve (no smile). Vols stored/returned in percent.
class VolCurve {
public:
  VolCurve() = default;
  VolCurve(const std::string &name, const Date &asOf)
      : name(name), asOf(asOf) {}

  void addVol(const std::string &tenor, double volPct);

  double getVol(const std::string &tenor) const;  // percent
  double getVolAtYears(double years) const;        // percent

  // Copy with every pillar shifted by shockPct (percent), e.g. +1.0 for a
  // +1 vol-point bump. Used for Vega.
  VolCurve bumped(double shockPct) const;

  void display() const;

  std::string name;
  Date asOf;

private:
  std::map<double, double> vols; // tenor years -> vol in percent
};

#endif // VOL_CURVE_H
