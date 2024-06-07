#include "Date.h"

Date operator+(const Date& d, double yearFrac)
{
    int numYear = yearFrac / 1.0;
    yearFrac = yearFrac - numYear / 1.0;
    int numMonth = yearFrac * 12;
    yearFrac = yearFrac - numMonth / 12.0;
    int numDays = yearFrac * 365;
    int _day = d.day + numDays;
    int _month = d.month + numMonth;
    int _year = d.year + numYear;

    switch (d.month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        if (_day > 31) {
            _day = _day - 31;
            _month++;
        }
    case 4: case 6: case 9: case 11:
        if (_day > 30) {
            _day = _day - 30;
            _month++;
        }
    case 2:
        if ((d.year % 400 == 0) || ((d.year % 100 != 0) && (d.year % 4 == 0))) {
            if (_day > 29) {
                _day = _day - 29;
                _month++;
            }
        }
        else {
            if (_day > 28) {
                _day = _day - 28;
                _month++;
            }
        }
    }

    if (_month > 12) {
        _month = _month - 12;
        _year++;
    }

    return Date(_year, _month, _day);
}

double operator-(const Date& d1, const Date& d2)
{
    int yearDiff = d1.year - d2.year;
    int monthDiff = (d1.month - d2.month);
    int dayDiff = d1.day - d2.day;
    return yearDiff + monthDiff / 12.0 + dayDiff / 365.0;
}

bool operator<(const Date& d1, const Date& d2)
{
    return (d1 - d2) < 0;
}

bool operator>(const Date& d1, const Date& d2)
{
    return (d1 - d2) > 0;
}

bool operator<=(const Date& d1, const Date& d2)
{
    return (d1 - d2) <= 0;
}
bool operator>=(const Date& d1, const Date& d2)
{
    return (d1 - d2) >= 0;
}

bool operator==(const Date& d1, const Date& d2)
{
    return (d1 - d2) == 0;
}

bool operator!=(const Date& d1, const Date& d2)
{
    return (d1 - d2) != 0;
}

std::ostream& operator<<(std::ostream& os, const Date& d)
{
    os << d.year << " " << d.month << " " << d.day;
    return os;
}

std::istream& operator>>(std::istream& is, Date& d)
{
    is >> d.year >> d.month >> d.day;
    return is;
}
