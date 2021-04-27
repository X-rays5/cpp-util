#include <string>
std::string BoolToString(bool Bool) {
    if (Bool == 1)
        return "true";
    if (Bool == 0)
        return "false";
    return "NULL";
}

bool StringToBool(std::string Bool) {
    if (Bool == "true" || Bool == "TRUE" || Bool == "1")
        return true;
    return false;
}