#include "Market.h"
#include <sstream>
#include <iostream>
#include <limits>
#include <cmath>

using namespace std;

void RateCurve::display() const {
    cout << "Rate curve: " << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << tenors[i].year << "-" << tenors[i].month << "-" << tenors[i].day << ": " << rates[i] << endl;
    }
    cout << endl;
}

void RateCurve::addRate(Date tenor, double rate) {
    tenors.push_back(tenor);
    rates.push_back(rate);
}

double RateCurve::getRate(Date tenor) const {
    if (tenors.empty())
        return 0;  // Handle empty tenors 

    // Function to convert Date to an integer for comparison
    auto dateToDays = [](const Date& d) {
        return d.year * 365 + d.month * 30 + d.day;  // Simplified conversion
        };

    int tenorDays = dateToDays(tenor);
    int closestLowerIndex = -1;
    int closestUpperIndex = -1;
    int minLowerDiff = std::numeric_limits<int>::max();
    int minUpperDiff = std::numeric_limits<int>::max();

    // Identify the closest lower and upper tenor indices
    for (size_t i = 0; i < tenors.size(); ++i) {
        int diff = dateToDays(tenors[i]) - tenorDays;
        if (diff <= 0 && std::abs(diff) < minLowerDiff) {
            minLowerDiff = std::abs(diff);
            closestLowerIndex = i;
        }
        else if (diff > 0 && diff < minUpperDiff) {
            minUpperDiff = diff;
            closestUpperIndex = i;
        }
    }

    // Edge case handling: if no upper or lower bound found, return closest available
    if (closestLowerIndex == -1 && closestUpperIndex != -1) {
        return rates[closestUpperIndex];
    }
    else if (closestLowerIndex != -1 && closestUpperIndex == -1) {
        return rates[closestLowerIndex];
    }
    else if (closestLowerIndex == -1 && closestUpperIndex == -1) {
        return 0;  // No volatilities available
    }

    // Perform linear interpolation
    double t0 = dateToDays(tenors[closestLowerIndex]);
    double v0 = rates[closestLowerIndex];
    double t1 = dateToDays(tenors[closestUpperIndex]);
    double v1 = rates[closestUpperIndex];

    double interpolatedVol = v0 + (tenorDays - t0) * (v1 - v0) / (t1 - t0);
    return interpolatedVol;
}

double Market::getMarketPrice() const {
    return bondPrices.at("SGD-GOV:");
}

void VolCurve::addVol(Date tenor, double vol) {
    tenors.push_back(tenor);
    vols.push_back(vol);
}

double VolCurve::getVol(Date tenor) const {
    if (tenors.empty())
        return 0;  // Handle empty tenors 

    // Function to convert Date to an integer for comparison
    auto dateToDays = [](const Date& d) {
        return d.year * 365 + d.month * 30 + d.day;  // Simplified conversion
        };

    int tenorDays = dateToDays(tenor);
    int closestLowerIndex = -1;
    int closestUpperIndex = -1;
    int minLowerDiff = std::numeric_limits<int>::max();
    int minUpperDiff = std::numeric_limits<int>::max();

    // Identify the closest lower and upper tenor indices
    for (size_t i = 0; i < tenors.size(); ++i) {
        int diff = dateToDays(tenors[i]) - tenorDays;
        if (diff <= 0 && std::abs(diff) < minLowerDiff) {
            minLowerDiff = std::abs(diff);
            closestLowerIndex = i;
        }
        else if (diff > 0 && diff < minUpperDiff) {
            minUpperDiff = diff;
            closestUpperIndex = i;
        }
    }

    // Edge case handling: if no upper or lower bound found, return closest available
    if (closestLowerIndex == -1 && closestUpperIndex != -1) {
        return vols[closestUpperIndex];
    }
    else if (closestLowerIndex != -1 && closestUpperIndex == -1) {
        return vols[closestLowerIndex];
    }
    else if (closestLowerIndex == -1 && closestUpperIndex == -1) {
        return 0;  // No volatilities available
    }

    // Perform linear interpolation
    double t0 = dateToDays(tenors[closestLowerIndex]);
    double v0 = vols[closestLowerIndex];
    double t1 = dateToDays(tenors[closestUpperIndex]);
    double v1 = vols[closestUpperIndex];

    double interpolatedVol = v0 + (tenorDays - t0) * (v1 - v0) / (t1 - t0);
    return interpolatedVol;
}

/*
double VolCurve::getVol(Date tenor) const {

    return 0;
}
*/

void VolCurve::display() const {
    cout << "Vol curve: " << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << tenors[i].year << "-" << tenors[i].month << "-" << tenors[i].day << ": " << vols[i] << endl;
    }
    cout << endl;
}

void Market::Print() const {
    cout << "Market as of: " << asOf.year << "-" << asOf.month << "-" << asOf.day << endl;

    cout << "Rate Curves:" << endl;
    for (const auto& curve : curves) {
        curve.second.display();
    }

    cout << "Vol Curves:" << endl;
    for (const auto& vol : vols) {
        vol.second.display();
    }

    cout << "Bond Prices:" << endl;
    for (const auto& bond : bondPrices) {
        cout << bond.first << ": " << bond.second << endl;
    }

    cout << "Stock Prices:" << endl;
    for (const auto& stock : stockPrices) {
        cout << stock.first << ": " << stock.second << endl;
    }
}

void Market::addCurve(const std::string& curveData) {
    stringstream curveDataStream(curveData);
    string curveName;
    getline(curveDataStream, curveName); // Read the curve name
    RateCurve rateCurve;
    rateCurve.name = curveName;
    string line;
    while (getline(curveDataStream, line)) {
        stringstream lineStream(line);
        string tenor;
        double rate;
        getline(lineStream, tenor, ':'); // Read the tenor
        lineStream >> rate;
        rate = rate / 100; // Convert percentage to decimal
        Date tenorDate;
        // Interpret tenors ON, 3M, etc., to Dates for simplification
        if (tenor == "ON") tenorDate = Date(0, 0, 1);
        else if (tenor == "3M") tenorDate = Date(0, 3, 0);
        else if (tenor == "6M") tenorDate = Date(0, 6, 0);
        else if (tenor == "9M") tenorDate = Date(0, 9, 0);
        else if (tenor == "1Y") tenorDate = Date(1, 0, 0);
        else if (tenor == "2Y") tenorDate = Date(2, 0, 0);
        else if (tenor == "5Y") tenorDate = Date(5, 0, 0);
        else if (tenor == "10Y") tenorDate = Date(10, 0, 0);
        rateCurve.addRate(tenorDate, rate);
    }
    
    curves[curveName] = rateCurve;
    //cout << curves[curveName].getRate() << endl;
}

void Market::addVolCurve(const std::string& volData) {
    stringstream volDataStream(volData);
    VolCurve volCurve;
    string line;
    while (getline(volDataStream, line)) {
        stringstream lineStream(line);
        string tenor;
        double vol;
        getline(lineStream, tenor, ':'); // Read the tenor
        lineStream >> vol;
        vol = vol / 100; // Convert percentage to decimal
        Date tenorDate;
        // Interpret tenors 1M, 3M, etc., to Dates for simplification
        if (tenor == "1M") tenorDate = Date(0, 1, 0);
        else if (tenor == "3M") tenorDate = Date(0, 3, 0);
        else if (tenor == "6M") tenorDate = Date(0, 6, 0);
        else if (tenor == "9M") tenorDate = Date(0, 9, 0);
        else if (tenor == "1Y") tenorDate = Date(1, 0, 0);
        else if (tenor == "2Y") tenorDate = Date(2, 0, 0);
        else if (tenor == "5Y") tenorDate = Date(5, 0, 0);
        else if (tenor == "10Y") tenorDate = Date(10, 0, 0);
        volCurve.addVol(tenorDate, vol);
    }
    vols["VolCurve"] = volCurve;
}

void Market::addBondPrice(const std::string& bondData) {
    stringstream bondDataStream(bondData);
    string bondName;
    double bondPrice;
    while (bondDataStream >> bondName >> bondPrice) {
        bondPrices[bondName] = bondPrice;
    }
}

void Market::addStockPrice(const std::string& stockData) {
    stringstream stockDataStream(stockData);
    string stockName;
    double stockPrice;
    while (stockDataStream >> stockName >> stockPrice) {
        stockPrices[stockName] = stockPrice;
    }
}

std::ostream& operator<<(std::ostream& os, const Market& mkt) {
    os << mkt.asOf.year << "-" << mkt.asOf.month << "-" << mkt.asOf.day;
    return os;
}

std::istream& operator>>(std::istream& is, Market& mkt) {
    is >> mkt.asOf.year >> mkt.asOf.month >> mkt.asOf.day;
    return is;
}
