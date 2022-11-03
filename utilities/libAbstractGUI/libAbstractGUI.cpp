#include "libAbstractGUI.h"

using namespace libAbstractGUI;

::std::string (*callback_get_text)(const abstract_widget) noexcept = nullptr;
void (*callback_set_text)(abstract_widget,
                          ::std::string_view text) noexcept = nullptr;

bool (*callback_is_enabled)(const abstract_widget) noexcept = nullptr;
void (*callback_set_enabled)(abstract_widget,
                             const bool set_enabled) noexcept = nullptr;

void (*callback_get_image)(const label, uint32_t *const dest_ARGB32,
                           const size_t dest_capacity_in_bytes, int *const rows,
                           int *const cols) noexcept = nullptr;
void (*callback_set_image)(label, const uint32_t *data_ARGB32, const int rows,
                           const int cols) noexcept = nullptr;
void (*callback_fill_label_with_color)(label, const uint32_t ARGB32) noexcept =
    nullptr;

bool (*callback_read_image_from_file)(::std::string_view filename,
                                      uint32_t *const dest_ARGB32,
                                      const size_t dest_capacity_in_bytes,
                                      int *const rows,
                                      int *const cols) noexcept = nullptr;
bool (*callback_write_image_to_file)(::std::string_view filename,
                                     const uint32_t *const data_ARGB32,
                                     const int rows,
                                     const int cols) noexcept = nullptr;

::std::string (*callback_get_open_filename)(
    ::std::string_view window_caption, ::std::string_view start_dir,
    ::std::string_view filter) noexcept = nullptr;
::std::vector<::std::string> (*callback_get_open_filenames)(
    abstract_widget parent_widget, ::std::string_view window_title,
    ::std::string_view start_dir, ::std::string_view filter) noexcept = nullptr;

bool (*callback_is_freezed)(const abstract_button) noexcept = nullptr;
void (*callback_set_freezed)(abstract_button,
                             const bool set_to_freezed) noexcept = nullptr;

void (*callback_get_progress_bar)(const progress_bar, int *const min,
                                  int *const max,
                                  int *const val) noexcept = nullptr;
void (*callback_set_progress_bar)(progress_bar, const int min, const int max,
                                  const int val) noexcept = nullptr;