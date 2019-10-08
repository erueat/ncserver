/*
Copyright (c) 2008-2015 Jesse Beder.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef BASE64_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define BASE64_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include <string>
#include <vector>

#include "yaml-cpp/dll.h"

namespace YAML {
YAML_CPP_API std::string EncodeBase64(const unsigned char *data,
                                      std::size_t size);
YAML_CPP_API std::vector<unsigned char> DecodeBase64(const std::string &input);

class YAML_CPP_API Binary {
 public:
  Binary(const unsigned char *data_, std::size_t size_)
      : m_data{}, m_unownedData(data_), m_unownedSize(size_) {}
  Binary() : Binary(nullptr, 0) {}

  bool owned() const { return !m_unownedData; }
  std::size_t size() const { return owned() ? m_data.size() : m_unownedSize; }
  const unsigned char *data() const {
    return owned() ? &m_data[0] : m_unownedData;
  }

  void swap(std::vector<unsigned char> &rhs) {
    if (m_unownedData) {
      m_data.swap(rhs);
      rhs.clear();
      rhs.resize(m_unownedSize);
      std::copy(m_unownedData, m_unownedData + m_unownedSize, rhs.begin());
      m_unownedData = 0;
      m_unownedSize = 0;
    } else {
      m_data.swap(rhs);
    }
  }

  bool operator==(const Binary &rhs) const {
    const std::size_t s = size();
    if (s != rhs.size())
      return false;
    const unsigned char *d1 = data();
    const unsigned char *d2 = rhs.data();
    for (std::size_t i = 0; i < s; i++) {
      if (*d1++ != *d2++)
        return false;
    }
    return true;
  }

  bool operator!=(const Binary &rhs) const { return !(*this == rhs); }

 private:
  std::vector<unsigned char> m_data;
  const unsigned char *m_unownedData;
  std::size_t m_unownedSize;
};
}  // namespace YAML

#endif  // BASE64_H_62B23520_7C8E_11DE_8A39_0800200C9A66
