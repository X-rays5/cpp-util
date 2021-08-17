void SaveFile(rapidjson::Document& json, const std::string& filename) {
        std::ofstream writer(filename);
        if (writer.is_open()) {
            if (!json.IsObject())
                json.SetObject();

            rapidjson::StringBuffer strbuf;
            strbuf.Clear();
            rapidjson::Writer<rapidjson::StringBuffer> jsonwriter(strbuf);
            json.Accept(jsonwriter);

            writer << strbuf.GetString();
            writer.close();
        } else {
            std::cerr << "Failed to open " << filename_ << " for writing\n";
        }
    }
