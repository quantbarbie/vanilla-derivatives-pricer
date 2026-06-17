#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace quant {

// Standard normal CDF via erfc (no <random>/Boost dependency).
inline double normCdf(double x) {
  return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Black-Scholes price for a European option on a non-dividend-paying asset.
//   S  spot, K strike, r continuously-compounded rate, vol annualised vol,
//   T  time to expiry in years, isCall true=call / false=put.
inline double blackScholes(double S, double K, double r, double vol, double T,
                           bool isCall) {
  if (T <= 0.0 || vol <= 0.0 || S <= 0.0 || K <= 0.0) {
    double intrinsic = isCall ? std::max(S - K, 0.0) : std::max(K - S, 0.0);
    return intrinsic;
  }
  const double sqrtT = std::sqrt(T);
  const double d1 = (std::log(S / K) + (r + 0.5 * vol * vol) * T) / (vol * sqrtT);
  const double d2 = d1 - vol * sqrtT;
  if (isCall) {
    return S * normCdf(d1) - K * std::exp(-r * T) * normCdf(d2);
  }
  return K * std::exp(-r * T) * normCdf(-d2) - S * normCdf(-d1);
}

// Black-76: option on a forward/rate F, discounted by df (= DF to expiry).
inline double black76(double F, double K, double vol, double T, double df,
                      bool isCall) {
  if (T <= 0.0 || vol <= 0.0 || F <= 0.0 || K <= 0.0) {
    double intrinsic = isCall ? std::max(F - K, 0.0) : std::max(K - F, 0.0);
    return df * intrinsic;
  }
  const double sqrtT = std::sqrt(T);
  const double d1 = (std::log(F / K) + 0.5 * vol * vol * T) / (vol * sqrtT);
  const double d2 = d1 - vol * sqrtT;
  if (isCall) {
    return df * (F * normCdf(d1) - K * normCdf(d2));
  }
  return df * (K * normCdf(-d2) - F * normCdf(-d1));
}

// Cox-Ross-Rubinstein binomial tree for an American option on an equity-like
// underlying. Returns the price for one unit of the underlying.
inline double crrAmerican(double S, double K, double r, double vol, double T,
                          bool isCall, int steps = 256) {
  if (T <= 0.0 || vol <= 0.0 || S <= 0.0) {
    return isCall ? std::max(S - K, 0.0) : std::max(K - S, 0.0);
  }
  const double dt = T / steps;
  const double u = std::exp(vol * std::sqrt(dt));
  const double d = 1.0 / u;
  const double disc = std::exp(-r * dt);
  const double p = (std::exp(r * dt) - d) / (u - d);

  std::vector<double> values(steps + 1);
  for (int i = 0; i <= steps; ++i) {
    const double ST = S * std::pow(u, steps - i) * std::pow(d, i);
    values[i] = isCall ? std::max(ST - K, 0.0) : std::max(K - ST, 0.0);
  }
  for (int step = steps - 1; step >= 0; --step) {
    for (int i = 0; i <= step; ++i) {
      const double cont = disc * (p * values[i] + (1.0 - p) * values[i + 1]);
      const double ST = S * std::pow(u, step - i) * std::pow(d, i);
      const double exercise =
          isCall ? std::max(ST - K, 0.0) : std::max(K - ST, 0.0);
      values[i] = std::max(cont, exercise);
    }
  }
  return values[0];
}

} // namespace quant

#endif // MATH_UTILS_H
