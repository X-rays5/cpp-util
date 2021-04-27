#include <string>
#include <algorithm>
void EraseChar(std::string& EraseFrom, char remove) {
    EraseFrom.erase(std::remove(EraseFrom.begin(), EraseFrom.end(), remove), EraseFrom.end());
}

#include <string>
std::string StringToUpper(std::string& convert) {
    std::transform(convert.begin(), convert.end(), convert.begin(), ::toupper);
    return convert;
}
std::string StringToLower(std::string& convert) {
    std::transform(convert.begin(), convert.end(), convert.begin(), ::tolower);
    return convert;
}
