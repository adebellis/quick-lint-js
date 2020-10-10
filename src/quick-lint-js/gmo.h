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

#include <cstdint>
#include <stdio.h>  // @@@
#include <string>   // @@@
#include <string_view>

namespace quick_lint_js {
// TODO(strager): Make this parser robust. gmo_parser currently trusts the input
// and assumes it is valid.
class gmo_parser {
 public:
  using offset_type = std::uint32_t;
  using word_type = std::uint32_t;

  explicit gmo_parser(const void *data, std::size_t data_size) noexcept
      : data_(reinterpret_cast<const std::uint8_t *>(data)),
        data_size_(data_size) {}

  word_type string_count() const noexcept {
    return this->read_word(/*offset=*/0x08);
  }

  std::string_view original_string_at(word_type index) const noexcept {
    return this->string_at(/*table_offset=*/this->original_strings_offset(),
                           /*index=*/index);
  }

  std::string_view translated_string_at(word_type index) const noexcept {
    return this->string_at(/*table_offset=*/this->translated_strings_offset(),
                           /*index=*/index);
  }

  std::string_view find_translation(std::string_view original) const noexcept {
    if (this->hash_table_size() == 0) {
      return this->find_translation_scanning(original);
    } else {
      return this->find_translation_hashing(original);
    }
  }

 private:
  std::string_view find_translation_scanning(std::string_view original) const
      noexcept {
    for (word_type i = 0; i < this->string_count(); ++i) {
      if (this->original_string_at(i) == original) {
        return this->translated_string_at(i);
      }
    }
    return original;
  }

  std::string_view find_translation_hashing(std::string_view original) const
      noexcept {
    offset_type hash_table_offset = this->read_word(/*offset=*/0x18);
    word_type hash_table_size = this->hash_table_size();

    word_type hash = this->hash_string(original);
    word_type bucket_index = hash % hash_table_size;
    word_type probe_increment = 1 + (hash % (hash_table_size - 2));

    for (;;) {
      word_type string_number =
          this->read_word(/*offset=*/hash_table_offset + bucket_index * 4);
      if (string_number == 0) {
        break;
      }
      word_type string_index = string_number - 1;
      if (this->original_string_at(string_index) == original) {
        return this->translated_string_at(string_number - 1);
      }
      bucket_index = (bucket_index + probe_increment) % hash_table_size;
    }
    return original;
  }

  static word_type hash_string(std::string_view s) noexcept {
    std::string s2(s);  // @@@ gross
    return __hash_string(s2.c_str());
  }

  // @@@ NOT MY CODE
  static unsigned long int __hash_string(const char *str_param) {
    constexpr int HASHWORDBITS = 32;
    unsigned long int hval, g;
    const char *str = str_param;

    /* Compute the hash value for the given string.  */
    hval = 0;
    while (*str != '\0') {
      hval <<= 4;
      hval += (unsigned char)*str++;
      g = hval & ((unsigned long int)0xf << (HASHWORDBITS - 4));
      if (g != 0) {
        hval ^= g >> (HASHWORDBITS - 8);
        hval ^= g;
      }
    }
    return hval;
  }

  word_type read_word(word_type offset) const noexcept {
    if (this->is_little_endian()) {
      return this->read_word_little_endian(offset);
    } else {
      return this->read_word_big_endian(offset);
    }
  }

  word_type read_word_little_endian(word_type offset) const noexcept {
    return (word_type{this->data_[offset + 0]} << 0) |
           (word_type{this->data_[offset + 1]} << 8) |
           (word_type{this->data_[offset + 2]} << 16) |
           (word_type{this->data_[offset + 3]} << 24);
  }

  word_type read_word_big_endian(word_type offset) const noexcept {
    return (word_type{this->data_[offset + 3]} << 0) |
           (word_type{this->data_[offset + 2]} << 8) |
           (word_type{this->data_[offset + 1]} << 16) |
           (word_type{this->data_[offset + 0]} << 24);
  }

  word_type hash_table_size() const noexcept {
    return this->read_word(/*offset=*/0x14);
  }

  offset_type original_strings_offset() const noexcept {
    return this->read_word(/*offset=*/0x0c);
  }

  offset_type translated_strings_offset() const noexcept {
    return this->read_word(/*offset=*/0x10);
  }

  std::string_view string_at(offset_type table_offset, word_type index) const
      noexcept {
    offset_type table_entry_offset = table_offset + index * 0x8;
    word_type length = this->read_word(table_entry_offset + 0x0);
    word_type offset = this->read_word(table_entry_offset + 0x4);
    return std::string_view(
        reinterpret_cast<const char *>(&this->data_[offset]), length);
  }

  bool is_little_endian() const noexcept {
    word_type magic = this->read_word_little_endian(/*offset=*/0);
    return magic == 0x950412de;
  }

  const std::uint8_t *data_;
  [[maybe_unused]] std::size_t data_size_;
};
}
