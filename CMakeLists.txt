cmake_minimum_required(VERSION 3.28)
project(Cplusplus_for_Financial_Engineering)

set(CMAKE_CXX_STANDARD 17)

add_executable(Cplusplus_for_Financial_Engineering
        AmericanTrade.h
        Bond.h
        Date.cpp
        Date.h
        EuropeanTrade.h
        main.cpp
        Market.cpp
        Market.h
        Payoff.h
        Pricer.cpp
        Pricer.h
        Swap.cpp
        Swap.h
        Trade.h
        TreeProduct.h
        Types.h)
