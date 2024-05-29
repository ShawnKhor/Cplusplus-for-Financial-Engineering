#include <fstream>
#include <ctime>
#include <chrono>
#include <iostream>
#include <sstream>
#include <functional>
#include <regex>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "Date.h"
#include "AmericanTrade.h"

using namespace std;

void readFromFile(const string& fileName, string& title, vector< pair<string, double> >& values) {
    string lineText, tmp_key, key, delimiter = ":";
    double value;
    size_t pos = 0;
    regex tr("ON|(\\d+[MY])");
    smatch m;

    ifstream MyReadFile(fileName);
    while (getline(MyReadFile, lineText)) {
        if (lineText.find(delimiter) == string::npos) {
            title.append(lineText);
        }
        else {
            // Check if first line consists of curve name
            while ((pos = lineText.find(":")) != string::npos) {
                tmp_key = lineText.substr(0, pos);
                // Check if keys are of different tenors
                // If they are, convert them into year fractions
                if (regex_match(tmp_key, m, tr)) {
                    switch (tmp_key.back()) {
                    case 'N':
                        key = to_string(1.0 / 365.0);
                        break;
                    case 'M':
                        key = to_string(stod(tmp_key.substr(0, tmp_key.size() - 1)) / 12.0);
                        break;
                    case 'Y':
                        key = to_string(stod(tmp_key.substr(0, tmp_key.size() - 1)));
                        break;
                    }
                }
                else {
                    key = tmp_key;
                }
                lineText.erase(0, pos + delimiter.length() + 1);
                // Check if values are in percentages
                if (lineText.back() == '%') {
                    value = stod(lineText.substr(0, lineText.size() - 1)) / 100;
                }
                else {
                    value = stod(lineText);
                }
                values.push_back(make_pair(key, value));
            }
        }
    }
    MyReadFile.close();
}

int main() {
    // Task 1: Create a market data object and update the market data from txt files 
    std::time_t t = std::time(0);
    auto now_ = std::localtime(&t);
    Date valueDate;
    valueDate.year = now_->tm_year + 1900;
    valueDate.month = now_->tm_mon + 1;
    valueDate.day = now_->tm_mday;

    Market mkt = Market(valueDate);


    // Read and add bond prices
    string bondname;
    std::string bondPricePath = "C://Users/sunit/OneDrive/Desktop/#SMU Stuff/QF633 C++/code_L5/code_L5/oop/bondPrice.txt";
    vector< pair<string, double> >bondData;
    
    readFromFile(bondPricePath,bondname, bondData);
    for (auto& bondPrice : bondData) {
        mkt.addBondPrice(bondPrice.first, bondPrice.second);
    }



    // Read and add stock prices
    string stockname;
    std::string stockPricePath = "C://Users/sunit/OneDrive/Desktop/#SMU Stuff/QF633 C++/code_L5/code_L5/oop/stockPrice.txt";
    vector< pair<string, double> >stockData;
    
    readFromFile(stockPricePath,stockname, stockData);
    for (auto& stockPrice : stockData) {
        mkt.addStockPrice(stockPrice.first, stockPrice.second);
    }


    // Read and add volatility data
    string volname;
    std::string volPath = "C://Users/sunit/OneDrive/Desktop/#SMU Stuff/QF633 C++/code_L5/code_L5/oop/vol.txt";
    vector< pair<string, double> >volData;
    
    readFromFile(volPath,volname, volData);
    if (volname.empty()) {
        volname = "ATM-Vol";
    }
    VolCurve volCurve(volname);
    for (auto& tenorVol : volData) {
        volCurve.addVol(valueDate + stod(tenorVol.first), tenorVol.second);
    }
    mkt.addVolCurve(volname, volCurve);


    // Read and add curve data
    string curvename;
    std::string curvePath = "C://Users/sunit/OneDrive/Desktop/#SMU Stuff/QF633 C++/code_L5/code_L5/oop/curve.txt";
    vector< pair<string, double> >curveData;
    
    readFromFile(curvePath,curvename, curveData);
    RateCurve usdSofr(curvename);
    for (auto& tenorRate : curveData) {
        usdSofr.addRate(valueDate + stod(tenorRate.first), tenorRate.second);
    }
    mkt.addCurve(curvename, usdSofr);


    mkt.Print();



    // Task 2: Create a portfolio of bond, swap, European option, American option
    // For each type, at least should have long/short, different tenor or expiry, different underlying
    // Total no less than 16 trades
    vector<Trade*> myPortfolio;

    //BONDS
    Trade* bond1 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
    myPortfolio.push_back(bond1);
    /*
    Trade* bond2 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
    myPortfolio.push_back(bond2);

    Trade* bond3 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
    myPortfolio.push_back(bond3);

    Trade* bond4 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
    myPortfolio.push_back(bond4);
    */


    //SWAPS
    Trade* swap1 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, 1);
    myPortfolio.push_back(swap1);
    /*
    Trade* swap2 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, 1);
    myPortfolio.push_back(swap2);

    Trade* swap3 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, 1);
    myPortfolio.push_back(swap3);

    Trade* swap4 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, 1);
    myPortfolio.push_back(swap4);
    */


    //American OPTIONS
    TreeProduct* A_option1 = new AmericanOption(Call, 100, Date(0, 2, 0));
    myPortfolio.push_back(A_option1);

    /*
    TreeProduct* A_option2 = new AmericanOption(Put, 100, Date(1, 0, 0));
    myPortfolio.push_back(A_option2);

    TreeProduct* A_option3 = new AmericanOption(BinaryCall, 100, Date(0, 2, 0));
    myPortfolio.push_back(A_option3);

    TreeProduct* A_option4 = new AmericanOption(BinaryPut, 100, Date(0, 2, 0));
    myPortfolio.push_back(A_option4);
    */

    //European Options
    TreeProduct* E_option1 = new EuropeanOption(Call, 100, Date(0, 2, 0));
    myPortfolio.push_back(E_option1);

    /*
    TreeProduct* E_option2 = new EuropeanOption(Put, 100, Date(1, 0, 0));
    myPortfolio.push_back(E_option2);

    TreeProduct* E_option3 = new EuropeanOption(BinaryCall, 100, Date(0, 2, 0));
    myPortfolio.push_back(E_option3);

    TreeProduct* E_option4 = new EuropeanOption(BinaryPut, 100, Date(0, 2, 0));
    myPortfolio.push_back(E_option4);
    */


    /* mkt.getVolCurve("");*/


     // Task 3: Create a pricer and price the portfolio, output the pricing result of each deal.
    Pricer* treePricer = new CRRBinomialTreePricer(10);
    for (auto trade : myPortfolio) {
        double pv = treePricer->Price(mkt, trade);
        cout << "Present Value for " << trade->getType() << ": " << pv << endl;
        // Log PV details out in a file
    }

    // Task 4: Analyzing pricing result
    // a) Compare CRR binomial tree result for a European option vs Black model
    // b) Compare CRR binomial tree result for an American option vs European option

    // Final
    cout << "Project build successfully!" << endl;
    return 0;
}
