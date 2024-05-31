#include "Market.h"
#include <sstream>
#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>

using namespace std;

void RateCurve::display() const {
    cout << "rate curve: " << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << tenors[i] << ": " << rates[i] * 100 << "%" << endl;
    }
    cout << endl;
}

void RateCurve::addRate(const Date& tenor, const double& rate) {
    //consider to check if tenor already exist
    //if exist, update rate else add rate to curve
    auto pos = find(tenors.begin(), tenors.end(), tenor);
    if (pos == tenors.end()) {
        tenors.push_back(tenor);
        rates.push_back(rate);
    }
    else {
        int idx = pos - tenors.begin();
        rates[idx] = rate;
    }
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

void VolCurve::display() const {
    cout << "vol curve: " << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << tenors[i] << ": " << vols[i] * 100 << "%" << endl;
    }
    cout << endl;
}

void VolCurve::addVol(const Date& tenor, const double& vol) {
    auto pos = find(tenors.begin(), tenors.end(), tenor);
    if (pos == tenors.end()) {
        tenors.push_back(tenor);
        vols.push_back(vol);
    }
    else {
        int idx = pos - tenors.begin();
        vols[idx] = vol;
    }
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

void Market::Print() const
{
    cout << "market asof: " << asOf << endl;

    for (auto& curve : curves) {
        curve.second.display();
    }
    for (auto& vol : vols) {
        vol.second.display();
    }
    /*
    add display for bond price and stock price
    */
    cout << "bond prices:" << endl;
    for (auto& bondPrice : bondPrices) {
        cout << bondPrice.first << ": " << bondPrice.second << endl;
    }
    cout << endl;
    cout << "stock prices:" << endl;
    for (auto& stockPrice : stockPrices) {
        cout << stockPrice.first << ": " << stockPrice.second << endl;
    }
    cout << endl;
}


void Market::addCurve(const std::string& curveData, const RateCurve& curve) {
    curves.emplace(curveData, curve);
}

void Market::addVolCurve(const std::string& volData, const VolCurve& curve) {
    vols.emplace(volData, curve);
}

void Market::addBondPrice(const std::string& bondData, double price) {
    bondPrices.emplace(bondData, price);
}

void Market::addStockPrice(const std::string& stockData, double price) {
    stockPrices.emplace(stockData, price);
}

std::ostream& operator<<(std::ostream& os, const Market& mkt) {
    os << mkt.asOf.year << "-" << mkt.asOf.month << "-" << mkt.asOf.day;
    return os;
}

std::istream& operator>>(std::istream& is, Market& mkt) {
    is >> mkt.asOf.year >> mkt.asOf.month >> mkt.asOf.day;
    return is;
}
