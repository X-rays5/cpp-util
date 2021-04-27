#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string stringify(rapidjson::Document& json) {
	rapidjson::StringBuffer strbuf;
	strbuf.Clear();

	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	json.Accept(writer);

	return strbuf.GetString();
}