#include <iostream>
#include <string>

class CCreditCardValidator {
public:
    explicit CCreditCardValidator(uint64_t card_num) : number_(card_num)
    {
    }

    bool isValid()
    {
        return (getSize(number_) >= 13 &&
                getSize(number_) <= 16) &&
               (prefixMatched(number_, 4) ||
                prefixMatched(number_, 5) ||
                prefixMatched(number_, 37) ||
                prefixMatched(number_, 6)) &&
               ((sumOfDoubleEvenPlace(number_) +
                 sumOfOddPlace(number_)) % 10 == 0);
    }
private:
    uint64_t number_;
private:
    uint64_t getDigit(uint64_t number)
    {
        if (number < 9)
            return number;
        return number / 10 + number % 10;
    }

    int getSize(uint64_t d)
    {
        return std::to_string(d).length();
    }

    uint64_t getPrefix(uint64_t number, int k)
    {
        if (getSize(number) > k)
        {
            return stol(std::to_string(number).substr(0, k));
        }
        return number;
    }


    bool prefixMatched(uint64_t number, int d)
    {
        return getPrefix(number, getSize(d)) == d;
    }

    int sumOfDoubleEvenPlace(uint64_t number)
    {
        int sum = 0;
        std::string num = std::to_string(number) ;
        for (int i = getSize(number) - 2; i >= 0; i -= 2)
            sum += getDigit(uint64_t(num[i] - '0') * 2);

        return sum;
    }

    int sumOfOddPlace(uint64_t number)
    {
        int sum = 0;
        std::string num = std::to_string(number) ;
        for (int i = getSize(number) - 1; i >= 0; i -= 2)
            sum += num[i] - '0';
        return sum;
    }
};