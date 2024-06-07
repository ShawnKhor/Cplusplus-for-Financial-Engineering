#pragma once

#include "Trade.h"

class Bond : public Trade {
public:
    Bond(Date start, Date end, double _notional, double _price, string _name) : Trade("BondTrade", start) {
        startDate = start;
        maturityDate = end;
        notional = _notional;
        tradePrice = _price;
        bondName = _name;
    }

    inline double Payoff(double marketPrice) const {
        return ((notional / tradePrice) * (marketPrice - tradePrice));
    }; // implement this
    string getName() const { return bondName; }
    Date GetExpiry() const override { return maturityDate;}
    double GetPrice() const override { return tradePrice;}
    string GetNotional() const override {return std::to_string(notional);}

private:
    double notional;
    double tradePrice;
    string bondName;
    Date startDate;
    Date maturityDate;
};