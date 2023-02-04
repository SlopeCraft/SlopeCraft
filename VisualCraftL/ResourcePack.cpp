#include "ParseResourcePack.h"

bool parse_single_state_expression(const char *const beg, const char *const end,
                                   resource_json::state *state) noexcept {
  const char *ptr_eq = nullptr;

  for (const char *cur = beg; cur < end; cur++) {
    if (*cur == '=') {
      if (ptr_eq != nullptr) {
        // multiple '=' in one block state
        return false;
      }

      ptr_eq = cur;
      break;
    }
  }

  if (ptr_eq == nullptr) {
    printf("\nError : state expression dosen\'t contain '=' : \"");

    for (const char *c = beg; c < end; c++) {
      printf("%c", *c);
    }
    printf("\"\n");
    return false;
  }

  state->key = std::string(beg, ptr_eq);
  state->value = std::string(ptr_eq + 1, end);

  return true;
}

bool process_block_state_list_in_id(
    const char *const beg, const char *const end,
    resource_json::state_list *states) noexcept {
  if (beg == end) {
    return true;
  }
  const char *cur_statement_beg = beg;
  for (const char *cur = beg;; cur++) {
    if (*cur == ',' || cur == end) {
      resource_json::state s;

      if (!parse_single_state_expression(cur_statement_beg, cur, &s)) {
        return false;
      }

      states->emplace_back(std::move(s));

      cur_statement_beg = cur + 1;
    }

    if (cur >= end)
      break;
  }

  return true;
}

bool resource_json::process_full_id(std::string_view full_id,
                                    std::string *namespace_name,
                                    std::string *pure_id,
                                    state_list *states) noexcept {
  if (namespace_name != nullptr)
    *namespace_name = "";
  if (pure_id != nullptr)
    pure_id->clear();
  if (states != nullptr)
    states->clear();

  if (full_id.find_first_of(':') != full_id.find_last_of(':')) {
    return false;
  }
  if (full_id.find_first_of('[') != full_id.find_last_of('[')) {
    return false;
  }
  if (full_id.find_first_of(']') != full_id.find_last_of(']')) {
    return false;
  }

  const size_t idx_colon = full_id.find_first_of(':');

  const size_t idx_left_bracket = full_id.find_first_of('[');

  const size_t idx_right_bracket = full_id.find_last_of(']');

  const bool have_left_bracket = (idx_left_bracket != full_id.npos);
  const bool have_right_bracket = (idx_right_bracket != full_id.npos);

  if (have_left_bracket != have_right_bracket)
    return false;

  if (idx_colon != full_id.npos && idx_colon > idx_left_bracket)
    return false;

  if (have_left_bracket && (idx_left_bracket >= idx_right_bracket)) {
    return false;
  }

  // get namespace name
  if (idx_colon != full_id.npos && namespace_name != nullptr) {
    *namespace_name = full_id.substr(idx_colon);
  }

  // get pure id
  const size_t pure_id_start_idx =
      (idx_colon == full_id.npos) ? (0) : (idx_colon + 1);
  const size_t pure_id_end_idx =
      (have_left_bracket) ? (idx_left_bracket) : (full_id.length());

  if (pure_id) {
    *pure_id =
        full_id.substr(pure_id_start_idx, pure_id_end_idx - pure_id_start_idx);
  }

  if (!have_left_bracket || states == nullptr) {
    return true;
  }

  if (!process_block_state_list_in_id(full_id.data() + idx_left_bracket + 1,
                                      full_id.data() + idx_right_bracket,
                                      states)) {
    return false;
  }

  return true;
}

std::variant<const block_model::model *, block_model::model>
resource_pack::find_model(const std::string &block_state_str,
                          VCL_face_t face_exposed, VCL_face_t *face_invrotated,
                          buffer_t &buffer) const noexcept {

  if (!resource_json::process_full_id(block_state_str, nullptr, &buffer.pure_id,
                                      &buffer.state_list)) {
    printf("\nError : invalid full block id that can not be parsed to a list "
           "of block states : \"%s\"\n",
           block_state_str.c_str());
    return nullptr;
  }

  constexpr bool display_statelist_here = false;
  if (display_statelist_here) {
    printf("statelist = [");
    for (const auto &i : buffer.state_list) {
      printf("%s=%s,", i.key.c_str(), i.value.c_str());
    }
    printf("]\n");
  }

  auto it_state = this->block_states.find(buffer.pure_id);
  /*
   if (it_state == this->block_states.end()) {
     it_state = this->block_states.find("block/" + buffer.pure_id);
   }
   */

  if (it_state == this->block_states.end()) {
    printf("\nError : undefined reference to block state whose pure block id "
           "is : \"%s\"  and full block id is : \"%s\"\n",
           buffer.pure_id.c_str(), block_state_str.c_str());
    return nullptr;
  }

  if (it_state->second.index() == 0) {
    resource_json::model_pass_t model =
        std::get<resource_json::block_states_variant>(it_state->second)
            .block_model_name(buffer.state_list);

    face_exposed = block_model::invrotate_y(face_exposed, model.y);
    face_exposed = block_model::invrotate_x(face_exposed, model.x);

    *face_invrotated = face_exposed;

    if (model.model_name == nullptr) {
      printf("\nError : No block model for full id : \"%s\", this is usually "
             "because block states mismatch.",
             block_state_str.c_str());
      return nullptr;
    }
    {
      std::string sv(model.model_name);
      const size_t idx_of_colon = sv.find_first_of(':');
      if (idx_of_colon != sv.npos) {
        buffer.pure_id = sv.substr(idx_of_colon + 1);
      } else {
        buffer.pure_id = sv;
      }
    }

    auto it_model = this->block_models.find(buffer.pure_id);
    if (it_model == this->block_models.end()) {
      it_model = this->block_models.find("block/" + buffer.pure_id);
    }

    if (it_model == this->block_models.end()) {
      printf(
          "Error : Failed to find block model for full id : \"%s\". Detail : "
          "undefined reference to model named \"%s\".\n",
          block_state_str.c_str(), model.model_name);
      return nullptr;
    }

    return &it_model->second;
  }
  // Here the block must be a multipart
  const auto &multipart =
      std::get<resource_json::block_state_multipart>(it_state->second);

  const auto models = multipart.block_model_names(buffer.state_list);
  for (const auto &md : models) {
    if (md.model_name == nullptr) {
      return nullptr;
    }
  }

  if (models.size() <= 0) {
    return nullptr;
  }

  block_model::model md;

  for (size_t mdidx = 0; mdidx < models.size(); mdidx++) {
    {
      std::string sv(models[mdidx].model_name);
      const size_t idx_of_colon = sv.find_first_of(':');
      if (idx_of_colon != sv.npos) {
        buffer.pure_id = sv.substr(idx_of_colon + 1);
      } else {
        buffer.pure_id = std::move(sv);
      }
    }

    auto it_model = this->block_models.find(buffer.pure_id);
    if (it_model == this->block_models.end()) {
      it_model = this->block_models.find("block/" + buffer.pure_id);
    }
    if (it_model == this->block_models.end()) {
      printf(
          "Error : Failed to find block model for full id : \"%s\". Detail : "
          "undefined reference to model named \"%s\".\n",
          block_state_str.c_str(), models[mdidx].model_name);
      return nullptr;
    }

    md.merge_back(it_model->second, models[mdidx].x, models[mdidx].y);
  }

  return std::move(md);
  // auto jt=this->block_models.find(it->second.)
}

bool resource_pack::compute_projection(const std::string &block_state_str,
                                       VCL_face_t face_exposed,
                                       block_model::EImgRowMajor_t *const img,
                                       buffer_t &buffer) const noexcept {

  std::variant<const block_model::model *, block_model::model> ret =
      this->find_model(block_state_str, face_exposed, &face_exposed, buffer);

  if (ret.index() == 0) {
    auto model = std::get<0>(ret);
    if (model == nullptr) {
      return false;
    }

    model->projection_image(face_exposed, img);
    return true;
  }

  block_model::model &md = std::get<1>(ret);

  md.projection_image(face_exposed, img);
  return true;
}