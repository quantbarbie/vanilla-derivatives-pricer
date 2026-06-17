#include "Market.h"

#include <algorithm>
#include <iostream>

void Market::Print() const {
  std::cout << "Market as of: " << asOf << std::endl;
  for (const auto &curve : curves) {
    curve.second.display();
  }
  for (const auto &vol : vols) {
    vol.second.display();
  }
  for (const auto &stock : stockPrices) {
    std::cout << "Stock: " << stock.first << ", Price: " << stock.second
              << std::endl;
  }
}

void Market::addCurve(const std::string &curveName, const RateCurve &curve) {
  curves[curveName] = curve;
}

void Market::addVolCurve(const std::string &curveName, const VolCurve &curve) {
  vols[curveName] = curve;
}

void Market::addStockPrice(const std::string &stockName, double price) {
  stockPrices[stockName] = price;
}

const RateCurve &Market::getCurve(const std::string &name) const {
  static const RateCurve empty;
  auto it = curves.find(name);
  return it != curves.end() ? it->second : empty;
}

const VolCurve &Market::getVolCurve(const std::string &name) const {
  static const VolCurve empty;
  auto it = vols.find(name);
  return it != vols.end() ? it->second : empty;
}

double Market::getStockPrice(const std::string &name) const {
  auto it = stockPrices.find(name);
  return it != stockPrices.end() ? it->second : 0.0;
}

bool Market::isRateUnderlying(const std::string &underlying) {
  std::string u = underlying;
  std::transform(u.begin(), u.end(), u.begin(), ::tolower);
  return u.find("sofr") != std::string::npos ||
         u.find("gov") != std::string::npos ||
         u.find("usd") != std::string::npos;
}

std::string Market::mapEquityUnderlying(const std::string &underlying) {
  std::string u = underlying;
  std::transform(u.begin(), u.end(), u.begin(), ::tolower);
  if (u == "appl" || u == "aapl" || u == "apple") {
    return "AAPL";
  }
  if (u == "sp500" || u == "spx" || u == "s&p500") {
    return "SP500";
  }
  if (u == "sti") {
    return "STI";
  }
  return "";
}

Market Market::bumpRates(double shockPct) const {
  Market out = *this;
  for (auto &kv : out.curves) {
    kv.second = kv.second.bumped(shockPct);
  }
  return out;
}

Market Market::bumpVols(double shockPct) const {
  Market out = *this;
  for (auto &kv : out.vols) {
    kv.second = kv.second.bumped(shockPct);
  }
  return out;
}

void Market::shockPrice(const std::string &underlying, double shock) {
  stockPrices[underlying] += shock;
}
