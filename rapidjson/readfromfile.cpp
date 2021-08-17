#include <fstream>
#include <iostream>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

rapidjson::Document ReadJsonFile(const std::string& filename) {
        if (!std::filesystem::exists(filename)) {
            std::ofstream createfile(filename);
            createfile << "{}"; // avoid document empty parse error
            createfile.close();
        }

        std::ifstream reader(filename);
        if (!reader.is_open()) {
            std::cerr << "Failed to open " << filename << " for reading\n";
            return {};
        }
        rapidjson::IStreamWrapper jsonfile(reader);
        rapidjson::Document json;
        if (json.ParseStream(jsonfile).HasParseError()) {
            std::cerr << "Parsing error " + static_cast<int>(json.GetParseError()) << "\n";
        }
        reader.close();
        return json;
}
