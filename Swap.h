#ifndef SWAP_H
#define SWAP_H

#include "Trade.h"

class Swap : public Trade {
public:
  using Trade::Trade;
  std::string getType() const override { return "Swap"; }

  // Vanilla fixed-for-float interest-rate swap. A positive notional receives
  // the fixed leg (strike rate) and pays float; negative reverses it.
  //   fixed leg PV  = N * strike * period * sum DF(t_i)
  //   float leg PV  = N * (DF(t_start) - DF(t_end))   [par float leg]
  //   swap PV       = fixed - float
  double pv(const Market &market) const override {
    const RateCurve &curve = market.discountCurve();
    const Date &asOf = market.asOfDate();

    const double tStart = std::max(start - asOf, 0.0);
    const double tEnd = end - asOf;
    if (tEnd <= tStart) {
      return 0.0;
    }
    const double period = (freq > 0.0) ? freq : 0.5;

    double annuity = 0.0;
    for (double t = tEnd; t > tStart + 1e-9; t -= period) {
      annuity += curve.getDfAtYears(t);
    }
    const double fixedLeg = notional * strike * period * annuity;
    const double floatLeg =
        notional * (curve.getDfAtYears(tStart) - curve.getDfAtYears(tEnd));
    return fixedLeg - floatLeg;
  }
};

#endif // SWAP_H
