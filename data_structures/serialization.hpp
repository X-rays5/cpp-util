#include <fstream>
#include <string>

namespace serialization {
	template<class T>
	void Dump(const std::string& file, T& target) {
		T* ptr = &target;
		std::ofstream writer(file, std::ios::binary);
		writer.write(reinterpret_cast<const char*>((void*)ptr), sizeof(target));
	}

	template<class T>
	void Load(const std::string& file, T& target) {
		T* ptr = &target;
		std::ifstream reader(file, std::ios::binary);
		reader.read(reinterpret_cast<char*>((void*)ptr), sizeof(target));
	}
}