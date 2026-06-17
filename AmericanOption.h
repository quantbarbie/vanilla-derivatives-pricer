#ifndef AMERICAN_OPTION_H
#define AMERICAN_OPTION_H

#include "MathUtils.h"
#include "Option.h"

class AmericanOption : public Option {
public:
  AmericanOption(int trade_id, const std::string &underlying,
                 const std::string &start, const std::string &end,
                 double notional, double freq, double strike,
                 const std::string &opt)
      : Option(trade_id, underlying, start, end, notional, freq, strike, opt,
               "American Option") {}

  double pv(const Market &market) const override {
    const double T = timeToExpiry(market);
    if (T <= 0.0) {
      return 0.0;
    }
    const RateCurve &curve = market.discountCurve();
    const VolCurve &volCurve = market.discountVol();
    const double r = curve.getRateAtYears(T) / 100.0;
    const double vol = volCurve.getVolAtYears(T) / 100.0;

    if (Market::isRateUnderlying(underlying)) {
      // No simple lattice for the forward rate here; approximate with the
      // European (Black-76) value, which is a lower bound for the American.
      const double F = curve.getRateAtYears(T) / 100.0;
      const double df = curve.getDfAtYears(T);
      const double unit = quant::black76(F, strike, vol, T, df, isCall());
      return notional * unit;
    }

    const std::string key = Market::mapEquityUnderlying(underlying);
    const double S = market.getStockPrice(key);
    const double unit = quant::crrAmerican(S, strike, r, vol, T, isCall());
    return notional * unit;
  }
};

#endif // AMERICAN_OPTION_H
