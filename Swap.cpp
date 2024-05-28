#include "Swap.h"
#include "Market.h"

/*
    implement this, using pv = annuity * (trade rate - market rate);
    approximately, annuity = sum of (notional * Discount factor at each period end);
    for a 2 year swap, with annual freq and notinal 100, annuity = 100 * Df(at year 1 end) + 100* Df(at year 2 end);
    Df = exp(-rT), r taken from curve;
    */



double Swap::getAnnuity(Market mkt) {
    RateCurve r = mkt.getCurve("USD-SOFR");
    double rate = r.getRate(Date(0, 1, 0));
    cout << rate << endl;
    // float df = exp(-rate * )

    // double annuity = notional * 
    return 0;
}










