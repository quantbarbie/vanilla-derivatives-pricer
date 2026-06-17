#ifndef BOND_H
#define BOND_H

#include "Trade.h"
#include <cmath>

class Bond : public Trade {
public:
  using Trade::Trade;
  std::string getType() const override { return "Bond"; }

  // Fixed-coupon bond: sum of discounted coupons (strike = annual coupon rate,
  // freq = period length in years) plus discounted principal at maturity.
  double pv(const Market &market) const override {
    const RateCurve &curve = market.discountCurve();
    const Date &asOf = market.asOfDate();

    const double maturity = end - asOf;
    if (maturity <= 0.0) {
      return 0.0;
    }
    const double period = (freq > 0.0) ? freq : 0.5;
    const double couponAmt = notional * strike * period;

    double value = 0.0;
    for (double t = maturity; t > 1e-9; t -= period) {
      value += couponAmt * curve.getDfAtYears(t);
    }
    value += notional * curve.getDfAtYears(maturity); // principal
    return value;
  }
};

#endif // BOND_H
