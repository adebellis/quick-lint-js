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

#ifndef QUICK_LINT_JS_TRANSLATION_H
#define QUICK_LINT_JS_TRANSLATION_H

#include <quick-lint-js/char8.h>

#define QLJS_TRANSLATE(...) (::quick_lint_js::translate(__VA_ARGS__))

namespace quick_lint_js {
const char8* translate(const char*);

void initialize_translations_from_environment(const char* locale_dir);
void initialize_translations_from_locale(const char* locale_dir,
                                         const char* locale_name);
}

#endif
