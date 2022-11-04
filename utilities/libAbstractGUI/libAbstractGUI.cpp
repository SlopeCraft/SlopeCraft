#include "libAbstractGUI.h"

using namespace libAbstractGUI;

abstract_widget (*callback_create_widget)(const widget_type) noexcept = nullptr;
void (*callback_destroy_widget)(abstract_widget) noexcept = nullptr;

::std::string (*callback_get_text)(const abstract_widget) noexcept = nullptr;
void (*callback_set_text)(abstract_widget,
                          ::std::string_view text) noexcept = nullptr;

bool (*callback_is_enabled)(const abstract_widget) noexcept = nullptr;
void (*callback_set_enabled)(abstract_widget,
                             const bool set_enabled) noexcept = nullptr;

void (*callback_set_freezed)(abstract_widget,
                             const bool set_to_freezed) noexcept = nullptr;

extern bool (*callback_is_checked)(const state_button) noexcept;
extern void (*callback_set_checked)(state_button,
                                    const bool set_to_checked) noexcept;

void (*callback_get_image)(const label, uint32_t *const dest_ARGB32,
                           const size_t dest_capacity_in_bytes, int *const rows,
                           int *const cols) noexcept = nullptr;
void (*callback_set_image)(label, const uint32_t *data_ARGB32, const int rows,
                           const int cols, const bool) noexcept = nullptr;
void (*callback_fill_label_with_color)(label, const uint32_t ARGB32) noexcept =
    nullptr;

// manipulate progress bar
void (*callback_get_progress_bar)(const progress_bar, int *const min,
                                  int *const max,
                                  int *const val) noexcept = nullptr;
void (*callback_set_progress_bar)(progress_bar, const int min, const int max,
                                  const int val) noexcept = nullptr;

// manipulate grid layout
void (*callback_get_gird_layout_size)(const grid_layout, int *const rows,
                                      int *const cols) noexcept = nullptr;
void (*callback_set_grid_layout_size)(grid_layout, const int rows,
                                      const int cols) noexcept = nullptr;
abstract_widget (*callback_get_widget_at)(const grid_layout, const int r,
                                          const int c) noexcept = nullptr;
void (*callback_set_widget_at)(grid_layout, abstract_widget, const int r,
                               const int c) noexcept = nullptr;

// image io
bool (*callback_read_image_from_file)(
    ::std::string_view filename, uint32_t *const dest_ARGB32,
    const size_t dest_capacity_in_bytes, int *const rows, int *const cols,
    bool *const is_row_major) noexcept = nullptr;
bool (*callback_write_image_to_file)(
    ::std::string_view filename, const uint32_t *const data_ARGB32,
    const int rows, const int cols, const bool is_row_major) noexcept = nullptr;
// File dialogs
::std::string (*callback_get_open_filename)(
    ::std::string_view window_caption, ::std::string_view start_dir,
    ::std::string_view filter) noexcept = nullptr;
::std::vector<::std::string> (*callback_get_open_filenames)(
    ::std::string_view window_title, ::std::string_view start_dir,
    ::std::string_view filter) noexcept = nullptr;

::std::string (*callback_get_exisiting_directory_name)(
    ::std::string_view window_caption,
    ::std::string_view start_dir) noexcept = nullptr;
::std::vector<::std::string> (*callback_get_exisiting_directory_names)(
    ::std::string_view window_caption,
    ::std::string_view start_dir) noexcept = nullptr;