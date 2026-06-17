#include "VolCurve.h"

#include <iostream>

void VolCurve::addVol(const std::string &tenor, double volPct) {
  vols[tenorToYears(tenor)] = volPct;
}

double VolCurve::getVolAtYears(double years) const {
  if (vols.empty()) {
    return 0.0;
  }
  if (years <= vols.begin()->first) {
    return vols.begin()->second;
  }
  if (years >= vols.rbegin()->first) {
    return vols.rbegin()->second;
  }
  auto hi = vols.lower_bound(years);
  auto lo = std::prev(hi);
  const double t0 = lo->first, v0 = lo->second;
  const double t1 = hi->first, v1 = hi->second;
  const double w = (years - t0) / (t1 - t0);
  return v0 + w * (v1 - v0);
}

double VolCurve::getVol(const std::string &tenor) const {
  return getVolAtYears(tenorToYears(tenor));
}

VolCurve VolCurve::bumped(double shockPct) const {
  VolCurve out = *this;
  for (auto &kv : out.vols) {
    kv.second += shockPct;
  }
  return out;
}

void VolCurve::display() const {
  std::cout << "Volatility Curve: " << name << " as of " << asOf << std::endl;
  for (const auto &kv : vols) {
    std::cout << "  " << kv.first << "Y: " << kv.second << "%" << std::endl;
  }
}
