#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
    Bond(Date start, Date end, double _notional, double _price, string _name): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
        bondName = _name;
    }

    inline double Payoff(double marketPrice) const { return ((notional/tradePrice) * (marketPrice - tradePrice));}; // implement this

private:
    double notional;
    double tradePrice;
    string bondName;
    Date startDate;
    Date maturityDate;
};