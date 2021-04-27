#include <string>
#include <algorithm>
void EraseChar(std::string& EraseFrom, char remove) {
	EraseFrom.erase(std::remove(EraseFrom.begin(), EraseFrom.end(), remove), EraseFrom.end());
}
