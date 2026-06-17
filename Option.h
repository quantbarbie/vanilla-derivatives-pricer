#ifndef OPTION_H
#define OPTION_H

#include "Trade.h"

// Common base for options. Holds the option label and helpers shared by the
// European and American pricers. Still abstract: pv() is implemented by the
// concrete European / American subclasses.
class Option : public Trade {
public:
  Option(int trade_id, const std::string &underlying, const std::string &start,
         const std::string &end, double notional, double freq, double strike,
         const std::string &opt, const std::string &option_type)
      : Trade(trade_id, underlying, start, end, notional, freq, strike, opt),
        option_type(option_type) {}

  std::string getType() const override { return option_type; }

protected:
  bool isCall() const { return opt == "call" || opt == "Call" || opt == "C"; }

  double timeToExpiry(const Market &market) const {
    return end - market.asOfDate();
  }

  std::string option_type;
};

#endif // OPTION_H
