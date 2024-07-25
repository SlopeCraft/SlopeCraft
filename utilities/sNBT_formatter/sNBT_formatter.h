//
// Created by Joseph on 2024/7/25.
//

#ifndef SLOPECRAFT_SNBT_FORMATTOR_H
#define SLOPECRAFT_SNBT_FORMATTOR_H

#include <ostream>
#include <iterator>
#include <string_view>
#include <ranges>

#include <nbt_tags.h>
#include <nbt_visitor.h>

namespace sNBT {

class sNBT_format_visitor : public nbt::const_nbt_visitor {
 private:
  std::ostream &os;

  auto iterator() const noexcept {
    return std::ostream_iterator<char>{this->os};
  }

 public:
  sNBT_format_visitor(std::ostream &dest) : os{dest} {}

  void visit(const nbt::tag_byte &b) override;

  void visit(const nbt::tag_short &s) override;

  void visit(const nbt::tag_int &i) override;

  void visit(const nbt::tag_long &l) override;

  void visit(const nbt::tag_float &f) override;

  void visit(const nbt::tag_double &d) override;

  void visit(const nbt::tag_byte_array &ba) override;

  void visit(const nbt::tag_int_array &ia) override;

  void visit(const nbt::tag_long_array &la) override;

  void visit(const nbt::tag_string &s) override;

  void visit(const nbt::tag_list &l) override;

  void visit(const nbt::tag_compound &c) override;
};
}  // namespace sNBT

#endif  // SLOPECRAFT_SNBT_FORMATTOR_H
