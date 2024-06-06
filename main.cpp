#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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

double normCDF(const double& x ) {
    return (1.0 + erf(x / sqrt(2.0))) / 2.0;
}

double blackModel(const double& S0, const double& K, const double& sigma, const double& rf,
                              const double& q, const double& T, const OptionType& optType) {
    double d1, d2, fwd, stdev;
    fwd = S0 * exp((rf-q) * T);
    stdev = sigma * sqrt(T);
    d1 = log(fwd/K) / stdev + stdev / 2.0;
    d2 = d1 - stdev;
    switch (optType) {
    case OptionType::Call:
        return exp(-rf * T) * (fwd*normCDF(d1) - K*normCDF(d2));
        break;
    case OptionType::Put:
        return exp(-rf * T) * (K*normCDF(-d2) - fwd*normCDF(-d1));
        break;
    case OptionType::BinaryCall:
        return exp(-rf * T) * normCDF(d1);
        break;
    case OptionType::BinaryPut:
        return exp(-rf * T) * (1 - normCDF(d1));
        break;
    default:
        throw invalid_argument("Unsupported OptionType");
    }
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
    std::string bondPricePath = "C:\\SMU\\Term 3\\QF633 - C++ for Financial Engineering\\Project_clone\\bondPrice.txt";
    vector< pair<string, double> >bondData;
    
    readFromFile(bondPricePath,bondname, bondData);
    for (auto& bondPrice : bondData) {
        mkt.addBondPrice(bondPrice.first, bondPrice.second);
    }



    // Read and add stock prices
    string stockname;
    std::string stockPricePath = "C:\\SMU\\Term 3\\QF633 - C++ for Financial Engineering\\Project_clone\\stockPrice.txt";
    vector< pair<string, double> >stockData;
    
    readFromFile(stockPricePath,stockname, stockData);
    for (auto& stockPrice : stockData) {
        mkt.addStockPrice(stockPrice.first, stockPrice.second);
    }


    // Read and add volatility data
    string volname;
    std::string volPath = "C:\\SMU\\Term 3\\QF633 - C++ for Financial Engineering\\Project_clone\\vol.txt";
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
    std::string curvePath = "C:\\SMU\\Term 3\\QF633 - C++ for Financial Engineering\\Project_clone\\curve.txt";
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
    Trade* bond1 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5, "SGD-GOV");
    myPortfolio.push_back(bond1);
    Trade* bond2 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 105.5, "USD-GOV");
    myPortfolio.push_back(bond2);

    Trade* bond3 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 107.5, "USD-GOV");
    myPortfolio.push_back(bond3);

    Trade* bond4 = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 99.5, "SGD-MAS-BILL");
    myPortfolio.push_back(bond4);


    //SWAPS
    Trade* swap1 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 1000, 1.1, mkt, 1, "swap1");
    myPortfolio.push_back(swap1);
    Trade* swap2 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 1000, 1.2, mkt, 1, "swap2");
    myPortfolio.push_back(swap2);

    Trade* swap3 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 1000, 1.3, mkt, 1, "swap3");
    myPortfolio.push_back(swap3);

    Trade* swap4 = new Swap(Date(2024, 1, 1), Date(2034, 1, 1), 1000, 1.4, mkt, 1, "swap4");
    myPortfolio.push_back(swap4);


    //American OPTIONS
    TreeProduct* A_option1 = new AmericanOption(Call, 100, Date(0, 2, 0), "AAPL");
    myPortfolio.push_back(A_option1);

    TreeProduct* A_option2 = new AmericanOption(Put, 100, Date(0, 2, 0), "AAPL");
    myPortfolio.push_back(A_option2);

    TreeProduct* A_option3 = new AmericanOption(BinaryCall, 100, Date(0, 2, 0), "AAPL");
    myPortfolio.push_back(A_option3);

    TreeProduct* A_option4 = new AmericanOption(BinaryPut, 100, Date(0, 2, 0), "APPL");
    myPortfolio.push_back(A_option4);

    //European Options
    TreeProduct* E_option1 = new EuropeanOption(Call, 100, Date(0, 2, 0), "APPL");
    myPortfolio.push_back(E_option1);

    TreeProduct* E_option2 = new EuropeanOption(Put, 100, Date(0, 2, 0), "APPL");
    myPortfolio.push_back(E_option2);

    TreeProduct* E_option3 = new EuropeanOption(BinaryCall, 100, Date(0, 2, 0), "APPL");
    myPortfolio.push_back(E_option3);

    TreeProduct* E_option4 = new EuropeanOption(BinaryPut, 100, Date(0, 2, 0), "APPL");
    myPortfolio.push_back(E_option4);


    /* mkt.getVolCurve("");*/


    // Task 3: Create a pricer and price the portfolio, output the pricing result of each deal.
    Pricer* treePricer = new CRRBinomialTreePricer(10);

    // uncomment for output file creation
    std::ofstream outputFile("C:\\SMU\\Term 3\\QF633 - C++ for Financial Engineering\\Project_clone\\output.txt");

    if (!outputFile) {
        std::cerr << "Unable to open file";
        return 1;
    }

    // Set column widths
    const int tradeTypeWidth = 15;
    const int tradeNameWidth = 20;
    const int pvWidth = 10;

    // Write headers with fixed width
    outputFile << std::left
        << std::setw(tradeTypeWidth) << "tradeType"
        << std::setw(tradeNameWidth) << "tradeName"
        << std::setw(pvWidth) << "pv"
        << '\n';

    for (const auto& trade : myPortfolio) {
        double pv = treePricer->Price(mkt, trade);
        std::string tradeName = trade->getName(); // Getting the tradename for clarity

        // Write each trade with fixed width columns
        outputFile << std::left
            << std::setw(tradeTypeWidth) << trade->getType()
            << std::setw(tradeNameWidth) << tradeName
            << std::setw(pvWidth) << pv
            << '\n';
    }

    outputFile.close(); // Close the output file
    cout << "Portfolio logged,open output.txt in same directory as main.cpp \n\n";

    // Task 4: Analyzing pricing result
    // a) Compare CRR binomial tree result for a European option vs Black model
    // b) Compare CRR binomial tree result for an American option vs European option

    // TASK 4 PART A - Eurpoean option vs black model
    cout << endl;
    cout << "TASK 4 part A: crr binomial vs Black model for european" << endl;
    double eur_k = 700;
    Date eur_maturity = Date(2026,5,21);
    string eur_stock = "APPL";
    TreeProduct* eur_aapl = new EuropeanOption(OptionType:: Call, eur_k, eur_maturity, eur_stock);

    // Black model
    double eur_rf = mkt.getCurve("USD-SOFR").getRate(eur_aapl->GetExpiry());
    double eur_sigma = mkt.getVolCurve("ATM-Vol").getVol(eur_aapl->GetExpiry());
    double eur_T = eur_aapl->GetExpiry() - valueDate;
    double eur_spot = mkt.getStockPrice(eur_aapl->getName());

    double black_price = blackModel(eur_spot,eur_k,eur_sigma,eur_rf,0,eur_T, OptionType::Call);
    cout << "European call option black model price: " << black_price << endl;


    auto nSteps = 20;
    double binomialPV;
    Pricer* nStepPricer;

    nStepPricer = new CRRBinomialTreePricer(nSteps);
    binomialPV = nStepPricer->Price(mkt, eur_aapl);
    cout << "binomial price: " << binomialPV << " and difference is: " << black_price - binomialPV << endl;

    // TASK 4 part B: american vs european call and put crr binomial tree
    cout<< endl;
    cout << "TASK 4 part B: american vs european call and put crr binomial tree" << endl;
    double eur_call_price;
    double eur_put_price;
    double amer_call_price;
    double amer_put_price;

    Trade* eur_call = new EuropeanOption(OptionType::Call, 700, Date(2024, 12, 21), "APPL");
    Trade* eur_put = new EuropeanOption(OptionType::Put, 700, Date(2026, 5, 21), "APPL");
    Trade* amer_call = new AmericanOption(OptionType::Call, 700, Date(2024, 12, 21), "APPL");
    Trade* amer_put = new AmericanOption(OptionType::Put, 700, Date(2026, 5, 21), "APPL");


    nStepPricer = new CRRBinomialTreePricer(nSteps);
    eur_call_price = nStepPricer->Price(mkt, eur_call);
    amer_call_price = nStepPricer->Price(mkt, amer_call);
    cout << "Binomial tree price for call options:" << endl;
    cout << "Eur call: " << eur_call_price << " and American Call: " << amer_call_price << endl;
    cout << "Difference: " << amer_call_price - eur_call_price << endl;

    nStepPricer = new CRRBinomialTreePricer(nSteps);
    eur_put_price = nStepPricer->Price(mkt, eur_put);
    amer_put_price = nStepPricer->Price(mkt, amer_put);
    cout << endl;
    cout << "Binomial tree price for put options:" << endl;
    cout << "Eur put: " << eur_put_price << " and American put: " << amer_put_price << endl;
    cout << "Difference: " << amer_put_price - eur_put_price << endl;

    // Final
    cout << "Project build successfully!" << endl;
    return 0;
}
