# Portfolio Pricing Engine (C++17)

A small trade-pricing library: loads a portfolio from CSV, builds market
snapshots (rate curve, vol curve, spot prices), and computes PV, DV01, Vega and
day-over-day PnL for bonds, swaps, and European/American options.

## Build & run

```bash
make            # or: cmake -B build && cmake --build build
./pricer        # expects trades.csv in the working directory
```

Requires a C++17 compiler. No external dependencies.

## Architecture

```
Trade (abstract)            pv(Market) is the only thing subclasses implement;
 ├─ Bond                    DV01 / Vega / PnL are derived in the base class by
 ├─ Swap                    bump-and-reprice, so risk is defined consistently.
 └─ Option (abstract)
     ├─ EuropeanOption      Black-Scholes (equity) / Black-76 (rate)
     └─ AmericanOption      CRR binomial tree (equity) / Black-76 (rate)

Market   -> RateCurve + VolCurve + spot prices, with bumpRates/bumpVols helpers
Portfolio-> aggregates PV/DV01/Vega/PnL and reports a hedge vs. risk limits
TradeFactory parses a row into the right Trade; MathUtils has the closed forms.
```

## Modeling assumptions

- Single-currency (USD) book; all trades discount off the USD curve.
- Zero rates are continuously compounded; discount factor `df = exp(-r*t)`.
- Day count is ACT/365 throughout.
- Equity options assume no dividends.
- Rate options (`usd-sofr`) are priced as a single European option on the
  curve-implied forward rate via Black-76; the American rate option uses the
  same value as a lower-bound approximation (no rate lattice).
- Swap float leg is valued at par: `N * (DF(start) - DF(end))`.
