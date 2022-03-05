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
#include <windows.h>
#include <objbase.h>
#include "winhttp_internal/httprequest.h"

// IID for IWinHttpRequest.
const IID IID_IWinHttpRequest =
  {
    0x06f29373,
    0x5c5a,
    0x4b54,
    {0xb0, 0x25, 0x6e, 0xf1, 0xbf, 0x8a, 0xbf, 0x0e}
  };

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

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
    long status_code = NULL;
    std::string url;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
  };

  class client {
  public:
    [[nodiscard]] static Req Request(Req req, bool https = true) {
      if (!IsHttp(req.url))
        throw std::runtime_error("Invalid URL");
      std::cout << "Requesting: " << req.url << std::endl;

      HRESULT hr;
      IWinHttpRequest* request = nullptr;

      hr = CoInitialize(nullptr);
      if (FAILED(hr))
        throw std::runtime_error("CoInitialize failed: " + std::to_string(HRESULT_CODE(hr)));

      CLSID clsid;
      hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1",&clsid);
      if (FAILED(hr))
        throw std::runtime_error("CLSIDFromProgID failed: " + std::to_string(HRESULT_CODE(hr)));

      hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_IWinHttpRequest, (void **)&request);
      if (FAILED(hr))
        throw std::runtime_error("CoCreateInstance failed: " + std::to_string(HRESULT_CODE(hr)));

      Open(request, req.url, MethodToString(req.method));
      SetHeaders(request, req.headers);
      SendRequest(request, req.body);

      BSTR bstr_res = nullptr;
      hr = request->get_ResponseText(&bstr_res);
      if (FAILED(hr))
        throw std::runtime_error("get_ResponseText failed: " + std::to_string(HRESULT_CODE(hr)));

      hr = request->get_Status(&req.status_code);
      if (FAILED(hr))
        throw std::runtime_error("get_Status failed: " + std::to_string(HRESULT_CODE(hr)));

      req.headers = GetHeaders(request);
      req.body = WstringToString(bstr_res);

      if (request)
        request->Release();
      if (bstr_res)
        SysFreeString(bstr_res);
      CoUninitialize();

      return req;
    }

  private:
    struct IsHttps {
      bool https;
    };

  private:
    static void Open(IWinHttpRequest* request, const std::string& url, const std::string& method) {
      HRESULT hr;
      VARIANT var_async;
      VariantInit(&var_async);
      V_VT(&var_async) = VT_BOOL;
      V_BOOL(&var_async) = VARIANT_FALSE;
      BSTR bstr_method = SysAllocString(StringToWstring(method).c_str());
      BSTR bstr_url = SysAllocString(StringToWstring(url).c_str());

      hr = request->Open(bstr_method, bstr_url, var_async);

      SysFreeString(bstr_method);
      SysFreeString(bstr_url);

      if (FAILED(hr))
        throw std::runtime_error("Open failed: " + std::to_string(HRESULT_CODE(hr)));
    }

    static void SendRequest(IWinHttpRequest* request, const std::string& body) {
      HRESULT hr;
      VARIANT var_body;
      VariantInit(&var_body);
      if (!body.empty()) {
        BSTR bstr_body = SysAllocString(StringToWstring(body).c_str());
        hr = request->SetRequestHeader(StringToWstring("Content-Length").data(), SysAllocString(StringToWstring(std::to_string(body.size())).c_str()));
        V_VT(&var_body) = VT_BSTR;
        var_body.bstrVal = bstr_body;

        hr = request->Send(var_body);
        SysFreeString(bstr_body);
        if (FAILED(hr))
          throw std::runtime_error("Send failed: " + std::to_string(HRESULT_CODE(hr)));
      } else {
        V_VT(&var_body) = VT_ERROR;
        hr = request->Send(var_body);
        if (FAILED(hr))
          throw std::runtime_error("Send failed: " + std::to_string(HRESULT_CODE(hr)));
      }
    }

    static void SetHeaders(IWinHttpRequest* request, const std::unordered_map<std::string, std::string>& headers) {
      for (auto&& [key, value] : headers) {
        BSTR bstr_key = SysAllocString(StringToWstring(key).c_str());
        BSTR bstr_value = SysAllocString(StringToWstring(value).c_str());
        HRESULT hr = request->SetRequestHeader(bstr_key, bstr_value);
        SysFreeString(bstr_key);
        SysFreeString(bstr_value);
        if (FAILED(hr))
          throw std::runtime_error("SetRequestHeader failed: " + std::to_string(HRESULT_CODE(hr)));
      }
    }

    static std::unordered_map<std::string, std::string> GetHeaders(IWinHttpRequest* request) {
      BSTR headers = nullptr;
      HRESULT hr = request->GetAllResponseHeaders(&headers);
      if (FAILED(hr))
        throw std::runtime_error("GetAllResponseHeaders failed: " + std::to_string(HRESULT_CODE(hr)));

      std::string str_headers = WstringToString(headers);
      SysFreeString(headers);

      std::unordered_map<std::string, std::string> res;
      std::vector<std::string> lines = Split(str_headers, '\n');
      for (auto&& line : lines) {
        std::vector<std::string> key_value = Split(line, ':');
        if (key_value.size() == 2) {
          res[Trim(key_value[0])] = Trim(key_value[1]);
        }
      }

      return res;
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

    static inline std::string Trim(const std::string& str) {
      std::string res = str;
      res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](int ch) {
        return !std::isspace(ch);
      }));
      res.erase(std::find_if(res.rbegin(), res.rend(), [](int ch) {
        return !std::isspace(ch);
      }).base(), res.end());
      return res;
    }

    static std::vector<std::string> Split(const std::string& str, char delimiter) {
      std::vector<std::string> res;
      std::stringstream ss(str);
      std::string item;
      while (std::getline(ss, item, delimiter)) {
        res.push_back(item);
      }
      return res;
    }

    static inline bool IsHttp(const std::string& url) {
      return !url.find("http://") || !url.find("https://");
    }

    inline static std::wstring StringToWstring(const std::string& str){
      MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
      std::wstring wstr(str.length(), 0);
      MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], str.length());
      return wstr;
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
