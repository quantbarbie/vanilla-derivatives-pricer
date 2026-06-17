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

The single design change that makes the project *functional* rather than a
stub: pricing takes a `const Market&`. Previously every `computePV` ignored its
argument and returned a hardcoded multiple of notional.

## What was fixed

- **Pricing actually uses market data.** Bonds discount their cashflows off the
  curve; swaps value fixed-vs-float legs; options use spot/vol/rate. The old
  `notional * 0.95`-style placeholders are gone.
- **Risk is real.** DV01 = PV change for a -1bp parallel curve shift; Vega = PV
  change for a +1 vol-point shift; PnL = PV(day2) - PV(day1). All by
  bump-and-reprice, centralised in `Trade`.
- **Date arithmetic.** `operator-` now uses an exact serial-day count
  (days-from-civil) for ACT/365 year fractions instead of the broken
  `month/12 + day/365` mix. Added `Date::fromString`, `serial()`, real
  `dateAddTenor`, and `tenorToYears`.
- **Curve interpolation & discounting.** `RateCurve`/`VolCurve` now interpolate
  linearly in time (sorted pillars, flat extrapolation). `getDf` uses the
  maturity-matched rate, not just `ON`.
- **Underlying mapping.** CSV underlyings (`usd-gov`, `usd-sofr`, `appl`,
  `sp500`, `sti`) are mapped to market keys (`USD`, `AAPL`, `SP500`, `STI`), so
  trades find their market data.
- **`maximizePnl`'s `* 2` nonsense removed.** Replaced with `Portfolio::reportHedge`,
  which reports the offsetting DV01/Vega needed to flatten the book.
- **Cleanups.** No-op `calculatePnL`/`computePnL` placeholders removed,
  `time_t`-based dates replaced by the `Date` class, modern overrides, `explicit`
  ctors, and a warning-clean build under `-Wall -Wextra`.

## Modeling assumptions (kept deliberately simple)

- Single-currency (USD) book; all trades discount off the USD curve.
- Zero rates are continuously compounded; discount factor `df = exp(-r*t)`.
- Day count is ACT/365 throughout.
- Equity options assume no dividends.
- Rate options (`usd-sofr`) are priced as a single European option on the
  curve-implied forward rate via Black-76; the American rate option uses the
  same value as a lower-bound approximation (no rate lattice).
- Swap float leg is valued at par: `N * (DF(start) - DF(end))`.
