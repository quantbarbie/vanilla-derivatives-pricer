#ifndef MARKET_H
#define MARKET_H

#include "Date.h"
#include "RateCurve.h"
#include "VolCurve.h"
#include <string>
#include <unordered_map>

class Market {
public:
  Market() = default;
  explicit Market(const Date &now) : asOf(now) {}

  void Print() const;
  void addCurve(const std::string &curveName, const RateCurve &curve);
  void addVolCurve(const std::string &curveName, const VolCurve &curve);
  void addStockPrice(const std::string &stockName, double price);

  const Date &asOfDate() const { return asOf; }

  // Lookups. Missing keys return a reference to a shared empty object / 0.0
  // rather than silently inserting, so the market stays const-correct.
  const RateCurve &getCurve(const std::string &name) const;
  const VolCurve &getVolCurve(const std::string &name) const;
  double getStockPrice(const std::string &name) const;

  // The book is single-currency (USD); these resolve the curve/vol a trade
  // should be priced against.
  const RateCurve &discountCurve() const { return getCurve("USD"); }
  const VolCurve &discountVol() const { return getVolCurve("USD"); }

  // Map a trade underlying (as written in the CSV) to a market stock key.
  // Returns "" if the underlying is not an equity-like spot.
  static std::string mapEquityUnderlying(const std::string &underlying);
  static bool isRateUnderlying(const std::string &underlying);

  // Scenario helpers (return a modified copy; the original is untouched).
  Market bumpRates(double shockPct) const;
  Market bumpVols(double shockPct) const;
  void shockPrice(const std::string &underlying, double shock);

private:
  std::unordered_map<std::string, RateCurve> curves;
  std::unordered_map<std::string, VolCurve> vols;
  std::unordered_map<std::string, double> stockPrices;
  Date asOf;
};

#endif // MARKET_H
