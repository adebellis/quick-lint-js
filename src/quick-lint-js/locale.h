// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew Glazar
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef QUICK_LINT_JS_LOCALE_H
#define QUICK_LINT_JS_LOCALE_H

#include <cstring>
#include <string>
#include <vector>

namespace quick_lint_js {
template <class T>
struct locale_entry {
  char locale[10];
  T data = T();

  bool valid() const noexcept { return this->locale[0] != '\0'; }

  bool has_locale_name(const char* name) const noexcept {
    return std::strcmp(this->locale, name) == 0;
  }
};

using gmo_file_ptr = locale_entry<const std::uint8_t*>;

template <class T>
const locale_entry<T>* find_locale_entry(const locale_entry<T>* files,
                                         const char* locale_name);
extern template const locale_entry<const std::uint8_t*>* find_locale_entry(
    const locale_entry<const std::uint8_t*>* files, const char* locale_name);
extern template const locale_entry<int>* find_locale_entry(
    const locale_entry<int>* files, const char* locale_name);

std::vector<std::string> locale_name_combinations(const char* locale_name);
}

#endif
