#include "Market.h"
#include "Portfolio.h"
#include "TradeFactory.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

// Trim whitespace (including CR from CRLF files) from both ends.
static std::string trim(const std::string &str) {
  const size_t first = str.find_first_not_of(" \t\r\n");
  const size_t last = str.find_last_not_of(" \t\r\n");
  return (first != std::string::npos) ? str.substr(first, last - first + 1) : "";
}

// Parse a double; treat "null"/empty as 0.0. Returns false on garbage.
static bool parseDouble(const std::string &str, double &value) {
  const std::string s = trim(str);
  if (s.empty() || s == "null") {
    value = 0.0;
    return true;
  }
  try {
    size_t pos = 0;
    value = std::stod(s, &pos);
    return pos == s.size();
  } catch (const std::exception &) {
    return false;
  }
}

static Portfolio loadPortfolioFromFile(const std::string &filename) {
  Portfolio portfolio;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return portfolio;
  }

  std::string line;
  std::getline(file, line); // header
  while (std::getline(file, line)) {
    if (trim(line).empty()) {
      continue;
    }
    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, ';')) {
      tokens.push_back(trim(token));
    }
    while (tokens.size() < 9) {
      tokens.push_back("null");
    }
    if (tokens.size() != 9) {
      std::cerr << "Invalid line format: " << line << std::endl;
      continue;
    }

    int trade_id = 0;
    try {
      trade_id = std::stoi(tokens[0]);
    } catch (const std::exception &) {
      std::cerr << "Bad trade id in line: " << line << std::endl;
      continue;
    }

    const std::string trade_type = tokens[1];
    const std::string underlying = tokens[2];
    const std::string start = tokens[3] == "null" ? "" : tokens[3];
    const std::string end = tokens[4] == "null" ? "" : tokens[4];
    double notional = 0, freq = 0, strike = 0;
    if (!parseDouble(tokens[5], notional) || !parseDouble(tokens[6], freq) ||
        !parseDouble(tokens[7], strike)) {
      std::cerr << "Error parsing numeric values in line: " << line << std::endl;
      continue;
    }
    const std::string opt = tokens[8] == "null" ? "" : tokens[8];

    try {
      portfolio.addTrade(TradeFactory::createTrade(
          trade_id, trade_type, underlying, start, end, notional, freq, strike,
          opt));
    } catch (const std::exception &e) {
      std::cerr << "Error creating trade: " << e.what() << std::endl;
    }
  }
  return portfolio;
}

static Market
createMarket(const Date &asOfDate,
             const std::vector<std::tuple<std::string, double>> &rates,
             const std::vector<std::tuple<std::string, double>> &vols,
             const std::vector<std::tuple<std::string, double>> &stocks) {
  Market market(asOfDate);
  RateCurve usdCurve("USD", asOfDate);
  VolCurve usdVol("USD", asOfDate);
  for (const auto &r : rates) {
    usdCurve.addRate(std::get<0>(r), std::get<1>(r));
  }
  for (const auto &v : vols) {
    usdVol.addVol(std::get<0>(v), std::get<1>(v));
  }
  market.addCurve("USD", usdCurve);
  market.addVolCurve("USD", usdVol);
  for (const auto &s : stocks) {
    market.addStockPrice(std::get<0>(s), std::get<1>(s));
  }
  return market;
}

int main() {
  Portfolio portfolio = loadPortfolioFromFile("trades.csv");
  std::cout << "==== Portfolio ====\n" << portfolio << std::endl;

  const Date asOf1(2024, 6, 20);
  const Date asOf2(2024, 6, 21);

  const std::vector<std::tuple<std::string, double>> rates1 = {
      {"ON", 5.56}, {"3M", 5.5}, {"6M", 5.45}, {"9M", 5.4}, {"1Y", 5.53},
      {"2Y", 5.34}, {"3Y", 5.1}, {"4Y", 4.9},  {"5Y", 4.75}, {"7Y", 4.5},
      {"10Y", 3.90}};
  const std::vector<std::tuple<std::string, double>> vols1 = {
      {"1M", 25.6}, {"3M", 20.1}, {"6M", 18.7}, {"9M", 15.4},
      {"1Y", 14.3}, {"2Y", 14.9}, {"5Y", 14.5}, {"10Y", 13.5}};
  const std::vector<std::tuple<std::string, double>> stocks1 = {
      {"AAPL", 652.0}, {"SP500", 5035.7}, {"STI", 3420.0}};
  const Market market1 = createMarket(asOf1, rates1, vols1, stocks1);

  const std::vector<std::tuple<std::string, double>> rates2 = {
      {"ON", 5.55}, {"3M", 5.52}, {"6M", 5.51}, {"9M", 5.4}, {"1Y", 5.52},
      {"2Y", 5.35}, {"3Y", 5.12}, {"4Y", 4.91}, {"5Y", 4.76}, {"7Y", 4.51},
      {"10Y", 3.85}};
  const std::vector<std::tuple<std::string, double>> vols2 = {
      {"1M", 25.0}, {"3M", 22.2}, {"6M", 19.0}, {"9M", 16.0},
      {"1Y", 15.0}, {"2Y", 14.5}, {"5Y", 14.0}, {"10Y", 13.0}};
  const std::vector<std::tuple<std::string, double>> stocks2 = {
      {"AAPL", 645.0}, {"SP500", 5040.7}, {"STI", 3401.0}};
  const Market market2 = createMarket(asOf2, rates2, vols2, stocks2);

  std::cout << "==== Day 1 Market ====" << std::endl;
  market1.Print();
  std::cout << "\n==== Day 2 Market ====" << std::endl;
  market2.Print();

  std::cout << "\n==== PV and Risk (DV01, Vega, PnL day1->day2) ====\n";
  portfolio.computePVAndRisk(market1, market2);

  std::cout << "==== Portfolio Totals ====\n";
  std::cout << "Total PV   : " << portfolio.totalPV(market1) << "\n";
  std::cout << "Total DV01 : " << portfolio.totalDV01(market1) << "\n";
  std::cout << "Total Vega : " << portfolio.totalVega(market1) << "\n";
  std::cout << "Total PnL  : " << portfolio.totalPnL(market1, market2) << "\n\n";

  std::cout << "==== Risk Limits / Hedge ====\n";
  portfolio.reportHedge(market1, /*dv01Limit=*/1'000'000.0,
                        /*vegaLimit=*/5'000.0);

  return 0;
}
