#include <string>
#include <sstream>
#include <cctype>

int StringToInt(const std::string& convert) {
    std::stringstream result;
    for (auto&& c : convert) {
        if (std::isdigit(c))
            result << c;
    }
    int converted;
    result >> converted;
    return converted;
}
