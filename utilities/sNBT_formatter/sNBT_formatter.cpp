//
// Created by Joseph on 2024/7/25.
//

#include "sNBT_formatter.h"

void sNBT::sNBT_format_visitor::visit(const nbt::tag_byte &b) {
  os << static_cast<int>(b.get()) << "b";
}  // We don't want to print a character

void sNBT::sNBT_format_visitor::visit(const nbt::tag_short &s) {
  this->os << s.get() << 's';
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_int &i) {
  this->os << i.get();
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_long &l) {
  this->os << l.get() << 'l';
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_float &f) {
  this->os << f.get() << 'f';
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_double &d) {
  this->os << d.get() << 'd';
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_byte_array &ba) {
  this->os << "[B;";
  for (size_t i = 0; i < ba.size() - 1; i++) {
    this->os << ba[i] << 'b';
  }
  if (ba.size() > 0) {
    this->os << ba[ba.size() - 1] << "b]";
  } else {
    os << "]";
  }
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_int_array &ia) {
  this->os << "[I;";
  for (size_t i = 0; i < ia.size() - 1; i++) {
    this->os << ia[i];
  }
  if (ia.size() > 0) {
    this->os << ia[ia.size() - 1] << ']';
  } else {
    os << "]";
  }
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_long_array &la) {
  this->os << "[L;";
  for (size_t i = 0; i < la.size() - 1; i++) {
    this->os << la[i] << 'l';
  }
  if (la.size() > 0) {
    this->os << la[la.size() - 1] << "l]";
  } else {
    os << "]";
  }
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_string &s) {
  if (s.get().find('\"') not_eq s.get().npos) {
    os << '\'' << s.get() << '\'';
  } else {
    os << '"' << s.get() << '"';
  }
}  // TODO: escape special characters

void sNBT::sNBT_format_visitor::visit(const nbt::tag_list &l) {
  os << "[";
  for (size_t idx = 0; idx < l.size(); idx++) {
    l[idx].get().accept(*this);
    if (idx < l.size() - 1) {
      os << ',';
    }
  }
  os << "]";
}

void sNBT::sNBT_format_visitor::visit(const nbt::tag_compound &c) {
  if (c.size() == 0) {
    this->os << "{}";
    return;
  }
  this->os << "{";
  size_t idx = 0;
  for (const auto &pair : c) {
    const auto &key = pair.first;
    const auto &val = pair.second;
    if (key.find(':') not_eq key.npos) {
      this->os << '\"' << key << '\"' << ':';
    } else {
      this->os << key << ':';
    }
    //    this->os << '\"' << key << "\":";
    val.get().accept(*this);
    if (idx < c.size() - 1) {
      this->os << ',';
    }
    idx++;
  }
  this->os << "}";
}