#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "Market.h"
#include "Trade.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

class Portfolio {
public:
  void addTrade(const std::shared_ptr<Trade> &trade) { trades.push_back(trade); }

  // Per-trade PV / DV01 / Vega (as of m1) and PnL between m1 and m2.
  void computePVAndRisk(const Market &m1, const Market &m2) const {
    std::cout << std::fixed << std::setprecision(2);
    for (const auto &trade : trades) {
      std::cout << "Trade ID: " << trade->getTradeId()
                << ", Type: " << trade->getType() << "\n"
                << "  PV   : " << trade->pv(m1) << "\n"
                << "  DV01 : " << trade->dv01(m1) << "\n"
                << "  Vega : " << trade->vega(m1) << "\n"
                << "  PnL  : " << trade->pnl(m1, m2) << "\n"
                << std::endl;
    }
  }

  double totalPV(const Market &m) const { return aggregate(m, &Trade::pv); }
  double totalDV01(const Market &m) const { return aggregate(m, &Trade::dv01); }
  double totalVega(const Market &m) const { return aggregate(m, &Trade::vega); }

  double totalPnL(const Market &m1, const Market &m2) const {
    double total = 0.0;
    for (const auto &trade : trades) {
      total += trade->pnl(m1, m2);
    }
    return total;
  }

  // Report current risk against limits and the parallel hedge that would flatten
  // the book (no magic numbers; just DV01/Vega divided through).
  void reportHedge(const Market &m, double dv01Limit, double vegaLimit) const {
    const double dv01 = totalDV01(m);
    const double vega = totalVega(m);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total DV01: " << dv01 << " (limit " << dv01Limit << ")\n";
    std::cout << "Total Vega: " << vega << " (limit " << vegaLimit << ")\n";

    if (std::abs(dv01) > dv01Limit) {
      std::cout << "  -> DV01 over limit by " << std::abs(dv01) - dv01Limit
                << "; hedge with an offsetting rate instrument of "
                << -dv01 << " DV01.\n";
    } else {
      std::cout << "  -> DV01 within limit.\n";
    }
    if (std::abs(vega) > vegaLimit) {
      std::cout << "  -> Vega over limit by " << std::abs(vega) - vegaLimit
                << "; hedge with an offsetting option position of " << -vega
                << " Vega.\n";
    } else {
      std::cout << "  -> Vega within limit.\n";
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Portfolio &p) {
    for (const auto &trade : p.trades) {
      trade->printDetails(os);
    }
    return os;
  }

private:
  double aggregate(const Market &m, double (Trade::*fn)(const Market &) const) const {
    double total = 0.0;
    for (const auto &trade : trades) {
      total += (trade.get()->*fn)(m);
    }
    return total;
  }

  std::vector<std::shared_ptr<Trade>> trades;
};

#endif // PORTFOLIO_H
