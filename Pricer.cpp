#include <cmath>
#include <string>
#include "Pricer.h"
#include "Market.h"


double Pricer::Price(Market& mkt, Trade* trade) {
  double pv;
  if (trade->getType() == "TreeProduct") {
    TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
    if (treePtr) { //check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  }
  else{
      double price{}; //get from market data
    //pv = trade->Payoff(price);
      pv = trade->Payoff(mkt.getMarketPrice());
  }

  return pv;
}

double BinomialTreePricer::PriceTree(Market& mkt, const TreeProduct& trade) {
  // model setup
  double T = trade.GetExpiry() - mkt.asOf;
  double dt = T / nTimeSteps;
  double stockPrice{}, vol{}, rate{};
  /*
  get these data for the deal from market object
  */
  cout << trade.GetExpiry() << endl;
  string day = to_string(trade.GetExpiry().day);
  string month = to_string(trade.GetExpiry().month);
  string year = to_string(trade.GetExpiry().year);

  //vol = mkt.getVolCurve("");

  //cout << vol << endl;

  ModelSetup(stockPrice, vol, rate, dt);

  //cout << nTimeSteps << endl;
  // initialize
  for (int i = 0; i <= nTimeSteps; i++) {
    states[i] = trade.Payoff( GetSpot(nTimeSteps, i) );
  }    
  
  // price by backward induction
  for (int k = nTimeSteps-1; k >= 0; k--)
    for (int i = 0; i <= k; i++) {
    // calculate continuation value
      double df = exp(-rate *dt);	  
      double continuation = df * (states[i]*GetProbUp() + states[i+1]*GetProbDown());
      // calculate the option value at node(k, i)
      states[i] = trade.ValueAtNode( GetSpot(k, i), dt*k, continuation);
    }

  return states[0];

}

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  currentSpot = S0;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
    u = std::exp( (rate - sigma*sigma/2) * dt + sigma * std::sqrt(dt) );
    d = std::exp( (rate - sigma*sigma/2) * dt - sigma * std::sqrt(dt) );
    p = (std::exp(rate*dt) -  d) / (u - d);
    currentSpot = S0;
}
