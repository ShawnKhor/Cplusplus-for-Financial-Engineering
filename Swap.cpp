#include "Swap.h"
#include "Market.h"
#include <cmath>
#include <vector>
#include <iostream>

/*
    implement this, using pv = annuity * (trade rate - market rate);
    approximately, annuity = sum of (notional * Discount factor at each period end);
    for a 2 year swap, with annual freq and notinal 100, annuity = 100 * Df(at year 1 end) + 100* Df(at year 2 end);
    Df = exp(-rT), r taken from curve;
    */

using namespace std;


double Swap::getAnnuity() const {
    std::vector<Date> payments;   //payments storage vector based on frequency
    double yearFrac = 1 / frequency;  //1 or 0.5 because freq can be 1 or 2

    for (Date day = startDate; day < maturityDate; day = day + yearFrac)
    {
        payments.push_back(day);
    }

    double sumDiscFacts = 0.0;

    for (const Date& day : payments)  //checking each payment to be before today (mkt.asOf) and adding it to discounts
    {
        if (day >= mkt.asOf)
        {
            double r = mkt.getCurve("USD-SOFR").getRate(day); 
             
            double t = day - mkt.asOf;
            sumDiscFacts += (exp(-(r * t)) * yearFrac); //following formula
        }
    }

    return notional * sumDiscFacts;
}










