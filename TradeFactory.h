#ifndef TRADE_FACTORY_H
#define TRADE_FACTORY_H

#include "AmericanOption.h"
#include "Bond.h"
#include "EuropeanOption.h"
#include "Swap.h"
#include "Trade.h"
#include <memory>
#include <stdexcept>

class TradeFactory {
public:
  static std::shared_ptr<Trade>
  createTrade(int trade_id, const std::string &trade_type,
              const std::string &underlying, const std::string &start,
              const std::string &end, double notional, double freq,
              double strike, const std::string &opt) {
    if (trade_type == "bond") {
      return std::make_shared<Bond>(trade_id, underlying, start, end, notional,
                                    freq, strike, opt);
    }
    if (trade_type == "swap") {
      return std::make_shared<Swap>(trade_id, underlying, start, end, notional,
                                    freq, strike, opt);
    }
    if (trade_type == "eur-opt") {
      return std::make_shared<EuropeanOption>(trade_id, underlying, start, end,
                                              notional, freq, strike, opt);
    }
    if (trade_type == "am-opt") {
      return std::make_shared<AmericanOption>(trade_id, underlying, start, end,
                                              notional, freq, strike, opt);
    }
    throw std::invalid_argument("Unknown trade type: " + trade_type);
  }
};

#endif // TRADE_FACTORY_H
