#ifndef SLOPECRAFT_UTILITIES_LIBABSTRACTGUI_LIBABSTRACTGUI_H
#define SLOPECRAFT_UTILITIES_LIBABSTRACTGUI_LIBABSTRACTGUI_H

#include <stdint.h>
#include <string>
#include <vector>

namespace libAbstractGUI {
/// @brief Supported language
enum class language : uint8_t { en_US, zh_CN };

enum class widget_type : size_t {
  unknown,
  radio_button,
  check_box,
  push_button,
  progress_bar,
  label,
  grid_layout
};

struct abstract_widget {
  void *ptr = nullptr;
  widget_type type = widget_type::unknown;

  inline bool is_null() const noexcept { return ptr == nullptr; }

  inline bool is_unknown() const noexcept {
    return type == widget_type::unknown;
  }
};

struct abstract_button : public abstract_widget {};

struct push_button : public abstract_button {
  push_button() { this->type = widget_type::push_button; }
};

struct state_button : public abstract_button {};
struct radio_button : public state_button {
  radio_button() { this->type = widget_type::radio_button; }
};
struct check_box : public state_button {
  check_box() { this->type = widget_type::check_box; }
};

struct progress_bar : public abstract_widget {
  progress_bar() { this->type = widget_type::progress_bar; }
};

struct label : public abstract_widget {
  label() { this->type = widget_type::label; }
};

struct grid_layout : public abstract_widget {
  grid_layout() { this->type = widget_type::grid_layout; }
};

// Global function ptrs starts with callback_ shoule be implemented by gui lib,
// and libAbstractGUI will call them when necessary. Their default value is
// nullptr
//
// Functions starts with on_ are implemented in this lib.
extern abstract_widget (*callback_create_widget)(const widget_type) noexcept;
extern void (*callback_destroy_widget)(abstract_widget) noexcept;

extern ::std::string (*callback_get_text)(const abstract_widget) noexcept;
extern void (*callback_set_text)(abstract_widget,
                                 ::std::string_view text) noexcept;

extern bool (*callback_is_enabled)(const abstract_widget) noexcept;
extern void (*callback_set_enabled)(abstract_widget,
                                    const bool set_enabled) noexcept;

extern void (*callback_set_freezed)(abstract_widget,
                                    const bool set_to_freezed) noexcept;

// manipulate buttons
extern bool (*callback_is_checked)(const state_button) noexcept;
extern void (*callback_set_checked)(state_button,
                                    const bool set_to_checked) noexcept;

// manipulate labels
extern void (*callback_get_image)(const label, uint32_t *const dest_ARGB32,
                                  const size_t dest_capacity_in_bytes,
                                  int *const rows, int *const cols) noexcept;
extern void (*callback_set_image)(label, const uint32_t *data_ARGB32,
                                  const int rows, const int cols,
                                  const bool is_row_major) noexcept;
extern void (*callback_fill_label_with_color)(label,
                                              const uint32_t ARGB32) noexcept;

// manipulate progress bar
extern void (*callback_get_progress_bar)(const progress_bar, int *const min,
                                         int *const max,
                                         int *const val) noexcept;
extern void (*callback_set_progress_bar)(progress_bar, const int min,
                                         const int max, const int val) noexcept;

// manipulate grid layout
extern void (*callback_get_gird_layout_size)(const grid_layout, int *const rows,
                                             int *const cols) noexcept;
extern void (*callback_set_grid_layout_size)(grid_layout, const int rows,
                                             const int cols) noexcept;
extern abstract_widget (*callback_get_widget_at)(const grid_layout, const int r,
                                                 const int c) noexcept;
extern void (*callback_set_widget_at)(grid_layout, abstract_widget, const int r,
                                      const int c) noexcept;

// image file io
extern bool (*callback_read_image_from_file)(
    ::std::string_view filename, uint32_t *const dest_ARGB32,
    const size_t dest_capacity_in_bytes, int *const rows, int *const cols,
    bool *const is_row_major) noexcept;
extern bool (*callback_write_image_to_file)(::std::string_view filename,
                                            const uint32_t *const data_ARGB32,
                                            const int rows, const int cols,
                                            const bool is_row_major) noexcept;
// File dialogs
extern ::std::string (*callback_get_open_filename)(
    ::std::string_view window_caption, ::std::string_view start_dir,
    ::std::string_view filter) noexcept;
extern ::std::vector<::std::string> (*callback_get_open_filenames)(
    ::std::string_view window_title, ::std::string_view start_dir,
    ::std::string_view filter) noexcept;

extern ::std::string (*callback_get_exisiting_directory_name)(
    ::std::string_view window_caption, ::std::string_view start_dir) noexcept;
extern ::std::vector<::std::string> (*callback_get_exisiting_directory_names)(
    ::std::string_view window_caption, ::std::string_view start_dir) noexcept;

} // namespace libAbstractGUI

#endif // SLOPECRAFT_UTILITIES_LIBABSTRACTGUI_LIBABSTRACTGUI_H