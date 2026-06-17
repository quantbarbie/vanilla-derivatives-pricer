#ifndef TRADE_H
#define TRADE_H

#include "Date.h"
#include "Market.h"
#include <ostream>
#include <string>

// Abstract base for every instrument. Subclasses implement pv(const Market&);
// DV01, Vega and PnL are provided here generically by bump-and-reprice so the
// risk definitions stay consistent across instrument types.
class Trade {
public:
  Trade(int trade_id, const std::string &underlying, const std::string &start,
        const std::string &end, double notional, double freq, double strike,
        const std::string &opt)
      : trade_id(trade_id), underlying(underlying),
        start(Date::fromString(start)), end(Date::fromString(end)),
        notional(notional), freq(freq), strike(strike), opt(opt) {}

  virtual ~Trade() = default;

  virtual std::string getType() const = 0;
  virtual double pv(const Market &market) const = 0;

  // DV01: PV change for a -1bp parallel shift of the rate curves.
  virtual double dv01(const Market &market) const {
    return pv(market.bumpRates(-0.01)) - pv(market);
  }

  // Vega: PV change for a +1 vol-point shift of the vol curves.
  virtual double vega(const Market &market) const {
    return pv(market.bumpVols(1.0)) - pv(market);
  }

  // PnL between two market snapshots (e.g. day 1 -> day 2).
  virtual double pnl(const Market &m1, const Market &m2) const {
    return pv(m2) - pv(m1);
  }

  virtual void printDetails(std::ostream &os) const {
    os << "Trade ID: " << trade_id << ", Type: " << getType()
       << ", Underlying: " << underlying << ", Notional: " << notional;
    if (start.isValid()) {
      os << ", Start: " << start;
    }
    if (end.isValid()) {
      os << ", End: " << end;
    }
    os << ", Freq: " << freq << ", Strike: " << strike;
    if (!opt.empty()) {
      os << ", Opt: " << opt;
    }
    os << std::endl;
  }

  int getTradeId() const { return trade_id; }

protected:
  int trade_id;
  std::string underlying;
  Date start;
  Date end;
  double notional;
  double freq;
  double strike;
  std::string opt;
};

#endif // TRADE_H
