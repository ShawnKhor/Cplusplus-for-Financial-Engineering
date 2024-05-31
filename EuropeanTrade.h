#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include <cassert>
#include "TreeProduct.h"
#include "Payoff.h"
#include "Types.h"

class EuropeanOption : public TreeProduct {
public:
    EuropeanOption() {};

    EuropeanOption(OptionType _optType, double _strike, const Date &_expiry, string _name) : optType(_optType),
                                                                                             strike(_strike),
                                                                                             expiryDate(_expiry),
                                                                                             name(_name) {};

    virtual double Payoff(double S) const { return PAYOFF::VanillaOption(optType, strike, S); }

    virtual const Date &GetExpiry() const { return expiryDate; }

    virtual double ValueAtNode(double S, double t, double continuation) const { return continuation; }

    virtual string getName() const { return name; }

private:
    OptionType optType;
    double strike;
    string name;
    Date expiryDate;
};

class EuroCallSpread : public EuropeanOption {
public:
    EuroCallSpread(double _k1, double _k2, const Date &_expiry, string _name) : strike1(_k1), strike2(_k2),
                                                                                expiryDate(_expiry), name(_name) {
        assert(_k1 < _k2);
    };

    virtual double Payoff(double S) const { return PAYOFF::CallSpread(strike1, strike2, S); };

    virtual const Date &GetExpiry() const { return expiryDate; };
private:
    double strike1;
    double strike2;
    string name;
    Date expiryDate;
};

#endif