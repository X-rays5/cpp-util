//
// Created by X-ray on 3/5/2022.
//

#pragma once

#ifndef WINHTTP_CLIENT_HPP
#define WINHTTP_CLIENT_HPP
#include <string>
#include <regex>
#include <unordered_map>
#include <utility>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

namespace winhttp {
  enum class Method {
    kGet,
    kPost,
    kPut,
    kDelete,
    kHead,
    kOptions,
    kTrace,
    kConnect,
    kPatch
  };

  struct Req {
    Req(Method m, std::string  u) : method(m), url(std::move(u))
    {
    }

    Method method;
    DWORD status_code = NULL;
    std::string url;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
  };

  class client {
  public:
    [[nodiscard]] static Req Request(Req req, bool https = true) {
      auto url = SplitUrl(req.url);
      std::string hostname = url.front;
      std::string path = url.back;

      HINTERNET http_session = WinHttpOpen(L"WinHttpClient", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS,NULL);
      if (http_session == nullptr)
        throw std::runtime_error("WinHttpOpen failed: " + std::to_string(GetLastError()));

      auto http_connection = HttpOrHttps(http_session, hostname, https);
      if (http_connection.conn == nullptr)
        throw std::runtime_error("HttpOrHttps failed: " + std::to_string(GetLastError()));

      HINTERNET http_request_handle = WinHttpOpenRequest(http_connection.conn, StringToWstring(MethodToString(req.method)).c_str(), StringToWstring(path).c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, http_connection.https ? WINHTTP_FLAG_SECURE : NULL);
      if (http_request_handle == nullptr)
        throw std::runtime_error("WinHttpOpenRequest failed: " + std::to_string(GetLastError()));

      auto headers = StringToWstring(MapToHeaders(req.headers));
      auto body = StringToWstring(req.body);
      BOOL http_result = WinHttpSendRequest(http_request_handle, headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headers.c_str(), headers.size(), (LPVOID)(body.empty() ? WINHTTP_NO_REQUEST_DATA : body.c_str()), body.size(), headers.size() + body.size(), NULL);
      if (!http_result)
        throw std::runtime_error("WinHttpSendRequest failed: " + std::to_string(GetLastError()));

      http_result = WinHttpReceiveResponse(http_request_handle, nullptr);
      if (!http_result)
        throw std::runtime_error("WinHttpReceiveResponse failed: " + std::to_string(GetLastError()));

      req.status_code = GetStatusCode(http_request_handle);
      req.headers = GetHeaders(http_request_handle);
      req.body = GetResponse(http_request_handle);
      return req;
    }

  private:
    struct IsHttps {
      HINTERNET conn;
      bool https;
    };

  private:
    inline static DWORD GetStatusCode(HINTERNET http_request_handle) {
      DWORD status_code = 0;
      DWORD status_code_size = sizeof(status_code);
      WinHttpQueryHeaders(http_request_handle,WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&status_code, &status_code_size, WINHTTP_NO_HEADER_INDEX);

      return status_code;
    }

    static std::unordered_map<std::string, std::string> GetHeaders(HINTERNET http_request_handle) {
      DWORD header_size = 0;
      if(!WinHttpQueryHeaders(http_request_handle, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &header_size, WINHTTP_NO_HEADER_INDEX)) {
        if (auto error = GetLastError(); error != ERROR_INSUFFICIENT_BUFFER)
          throw std::runtime_error("WinHttpQueryHeaders failed: " + std::to_string(error));

        LPVOID header_buff = new WCHAR[header_size / sizeof(WCHAR)];

        if(!WinHttpQueryHeaders(http_request_handle, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, header_buff, &header_size, WINHTTP_NO_HEADER_INDEX))
          throw std::runtime_error("WinHttpQueryHeaders failed: " + std::to_string(GetLastError()));

        std::unordered_map<std::string, std::string> headers;
        std::string header_str = WstringToString((WCHAR*)header_buff);
        std::vector<std::string> lines = Split(header_str, "\r\n");
        std::regex regex("(.*): (.*)");
        for (auto&& line : lines) {
          std::smatch match;
          if (std::regex_match(line, match, regex))
            headers[match[1].str()] = match[2].str();
        }

        return headers;
      }

      return {};
    }

    static std::string GetResponse(HINTERNET http_request_handle) {
      std::string result;
      DWORD incoming_data_size;
      LPSTR incoming_data = nullptr;
      do {
        incoming_data_size = 0;
        if (!WinHttpQueryDataAvailable(http_request_handle, &incoming_data_size))
          throw std::runtime_error("WinHttpQueryDataAvailable failed: " + std::to_string(GetLastError()));

        incoming_data = new char[incoming_data_size + 1];
        if (!incoming_data)
          throw std::runtime_error("Out of memory");

        ZeroMemory(incoming_data, incoming_data_size + 1);
        if (!WinHttpReadData(http_request_handle, (LPVOID)incoming_data, incoming_data_size, &incoming_data_size))
          throw std::runtime_error("WinHttpReadData failed: " + std::to_string(GetLastError()));

        result.append(incoming_data);

        delete[] incoming_data;
      } while(incoming_data_size > 0);

      return result;
    }

    static IsHttps HttpOrHttps(HINTERNET session, const std::string& url, bool https) {
      HINTERNET http_connection = nullptr;
      if (!https)
        goto http;

      http_connection = WinHttpConnect(session, StringToWstring(url).c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL);

      if (http_connection != nullptr)
        return {http_connection, true};

      http:
      http_connection = WinHttpConnect(session, StringToWstring(url).c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL);

      if (http_connection != nullptr)
        return {http_connection, false};

      return {nullptr, false};
    }

    inline static std::string MapToHeaders(const std::unordered_map<std::string, std::string>& headers) {
      std::string headers_str;
      for (auto& header : headers) {
        headers_str += header.first + ": " + header.second + "\r\n";
      }
      return headers_str;
    }

    inline static std::string CleanUrl(const std::string& url) {
      return std::regex_replace(url, std::regex(R"(https://|http://)"), "");;
    }

    struct Url {
      std::string front;
      std::string back;
    };

    // split url
    static inline Url SplitUrl(const std::string& url) {
      if (auto pos = url.find('/'); pos != std::string::npos)
        return {url.substr(0, pos), url.substr(pos)};

      return {url, "/"};
    }

    inline static const char* MethodToString(Method method) {
      switch (method) {
        case Method::kGet:
          return "GET";
        case Method::kPost:
          return "POST";
        case Method::kPut:
          return "PUT";
        case Method::kDelete:
          return "DELETE";
        case Method::kHead:
          return "HEAD";
        case Method::kOptions:
          return "OPTIONS";
        case Method::kTrace:
          return "TRACE";
        case Method::kConnect:
          return "CONNECT";
        case Method::kPatch:
          return "PATCH";
      }

      return {};
    }

    inline static std::vector<std::string> Split(const std::string& str, const std::string& delimiter) {
      std::vector<std::string> result;
      size_t pos = 0;
      size_t prev = 0;
      while ((pos = str.find(delimiter, prev)) != std::string::npos) {
        result.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.length();
      }
      result.push_back(str.substr(prev));
      return result;
    }

    inline static std::wstring StringToWstring(const std::string& str){
      std::wstring wsTmp(str.begin(), str.end());
      return wsTmp;
    }

    // convert wstring to string
    inline static std::string WstringToString(const std::wstring& wstr) {
      WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
      std::string str(wstr.size(), 0);
      WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], (int)str.size(), nullptr, nullptr);
      return str;
    }
  };
}
#endif //WINHTTP_CLIENT_HPP
