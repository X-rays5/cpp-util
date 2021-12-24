//
// Created by X-ray on 12/24/2021.
//

#pragma once

#ifndef STRING_SWITCH_JOAAT_HPP
#define STRING_SWITCH_JOAAT_HPP
namespace joaat {
  inline constexpr char ToLower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
  }

  inline constexpr std::uint32_t Hash(const char* str) {
    unsigned int hash = 0;
    for (; *str; ++str) {
      hash += ToLower(*str);
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
  }
}

#define STR_SWITCH(str) ::joaat::Hash(str)
#define JOAAT_CONSTEXPR(str) constexpr auto res = ::joaat::Hash(str); res
#endif //STRING_SWITCH_JOAAT_HPP
