#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
    Bond(Date start, Date end, double _notional, double _price): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
    }

    inline double Payoff(double marketPrice) const { return ((notional/tradePrice) * (marketPrice - tradePrice));}; // implement this

private:
    double notional;
    double tradePrice;
    Date startDate;
    Date maturityDate;
};