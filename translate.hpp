//
// Created by X-ray on 8/16/2021.
//
#pragma once
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class translate_exception {
public:
    explicit translate_exception(std::string except) : what_(std::move(except))
    {
    }

    std::string what() {
        return what_;
    }
private:
    std::string what_;
};

class translate {
public:
    explicit translate(std::string filename, bool saveondeconstruct) : filename_(std::move(filename)), saveondeconstruct_(saveondeconstruct)
    {
        ReloadTranslationFile();
    }

    ~translate() {
        if (saveondeconstruct_)
            SaveFile(filename_);
    }

    void SetTranslationFile(std::string filename) {
        filename_ = std::move(filename);
        ReloadTranslationFile();
    }

    std::string GetTranslationFile() {
        return filename_;
    }

    void ReloadTranslationFile() {
        auto json = ReadJsonFile(filename_);
        if (!json.IsObject())
            json.SetObject();

        json_ = std::move(json);
    }

    std::string Get(std::string key, std::string default_v) {
        if (json_.HasMember(key.c_str())) {
            if (json_[key.c_str()].IsString()) {
                return json_[key.c_str()].GetString();
            } else {
                throw translate_exception("translation is not a string");
            }
        } else {
            Set(key, default_v);
            return default_v;
        }
    }

    void Set(const std::string& key, const std::string& value) {
        if (!json_.IsObject())
            json_.SetObject();

        if (json_.HasMember(key.c_str())) {
            json_[key.c_str()].SetString(value.c_str(), value.size());
        }

        rapidjson::Value index(key.c_str(), key.size(), json_.GetAllocator());
        rapidjson::Value val(value.c_str(), value.size(), json_.GetAllocator());
        json_.AddMember(index, val, json_.GetAllocator());
    }

    void SaveFile(const std::string& filename) {
        std::ofstream writer(filename);
        if (writer.is_open()) {
            if (!json_.IsObject())
                json_.SetObject();

            rapidjson::StringBuffer strbuf;
            strbuf.Clear();
            rapidjson::Writer<rapidjson::StringBuffer> jsonwriter(strbuf);
            json_.Accept(jsonwriter);

            writer << strbuf.GetString();
            writer.close();
        } else {
            std::cerr << "Failed to open " << filename_ << " for writing\n";
        }
    }
private:
    std::string filename_;
    rapidjson::Document json_;
    bool saveondeconstruct_ = false;

private:
    static rapidjson::Document ReadJsonFile(const std::string& filename) {
        if (!std::filesystem::exists(filename)) {
            std::ofstream createfile(filename);
            createfile << "{}"; // avoid document empty parse error
            createfile.close();
        }

        std::ifstream reader(filename);
        if (!reader.is_open())
            throw translate_exception("Failed to open file for reading");
        rapidjson::IStreamWrapper jsonfile(reader);
        rapidjson::Document json;
        if (json.ParseStream(jsonfile).HasParseError()) {
            reader.close();
            throw translate_exception("Parsing error " + std::to_string(static_cast<int>(json.GetParseError())));
        }
        reader.close();
        return json;
    }
};
