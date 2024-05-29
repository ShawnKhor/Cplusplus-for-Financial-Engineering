#ifndef MARKET_H
#define MARKET_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Date.h"

using namespace std;

class RateCurve {
public:
    RateCurve() {};
    RateCurve(const string& _name) : name(_name) {};
    void addRate(const Date& tenor, const double& rate);
    double getRate(Date tenor) const; // implement this function using linear interpolation
    void display() const;
    std::string name;

private:
    vector<Date> tenors;
    vector<double> rates;
};

class VolCurve { // atm vol curve without smile
public:
    VolCurve() {}
    VolCurve(const string& _name) : name(_name) {};
    void addVol(const Date& tenor,const double& rate); // implement this
    double getVol(Date tenor) const; // implement this function using linear interpolation
    void display() const; // implement this

private:
    string name;
    vector<Date> tenors;
    vector<double> vols;
};

class Market {
public:
    Date asOf;
    Market(const Date& now) : asOf(now) {}
    void Print() const;
    void addCurve(const std::string& curveData, const RateCurve& curve);
    void addVolCurve(const std::string& volData, const VolCurve& curve);
    void addBondPrice(const std::string& bondData, double price);
    void addStockPrice(const std::string& stockData, double price);
    double getMarketPrice() const;

    inline RateCurve getCurve(const string& name) { return curves[name]; }
    inline VolCurve getVolCurve(const string& name) { return vols[name]; }
    inline double getStockPrice(const string& name) const { return stockPrices.at(name); };
    inline double getBondPrice(const string& name) const { return bondPrices.at(name); };

private:
    unordered_map<string, VolCurve> vols;
    unordered_map<string, RateCurve> curves;
    unordered_map<string, double> bondPrices;
    unordered_map<string, double> stockPrices;
};

std::ostream& operator<<(std::ostream& os, const Market& obj);
std::istream& operator>>(std::istream& is, Market& obj);

#endif
