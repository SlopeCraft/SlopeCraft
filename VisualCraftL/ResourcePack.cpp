#include "ParseResourcePack.h"
#include <utilities/Schem/bit_shrink.h>

void traits_format_convert(
    std::vector<std::pair<std::string, std::string>> *src,
    resource_json::state_list *const dst) noexcept {
  dst->resize(src->size());

  for (size_t i = 0; i < src->size(); i++) {
    dst->at(i).key = std::move(src->at(i).first);
    dst->at(i).value = std::move(src->at(i).second);
  }
}

const block_model::model *
resource_pack::find_model(const std::string &block_state_str,
                          VCL_face_t face_exposed, VCL_face_t *face_invrotated,
                          buffer_t &buffer) const noexcept {

  if (!process_block_id(block_state_str, &buffer.pure_id, &buffer.traits)) {
    printf("\nError : invalid full block id that can not be parsed to a list "
           "of block states : \"%s\"\n",
           block_state_str.c_str());
    return nullptr;
  }

  auto it_state = this->block_states.find(buffer.pure_id);

  if (it_state == this->block_states.end()) {
    printf("\nError : undefined reference to block state whose pure block id "
           "is : \"%s\"  and full block id is : \"%s\"\n",
           buffer.pure_id.c_str(), block_state_str.c_str());
    return nullptr;
  }

  traits_format_convert(&buffer.traits, &buffer.state_list);
  resource_json::model_pass_t model =
      it_state->second.block_model_name(buffer.state_list);

  face_exposed = block_model::invrotate_y(face_exposed, model.y);
  face_exposed = block_model::invrotate_x(face_exposed, model.x);

  *face_invrotated = face_exposed;

  if (model.model_name == nullptr) {
    printf("\nError : No block model for full id : \"%s\"",
           block_state_str.c_str());
    return nullptr;
  }

  auto it_model = this->block_models.find(model.model_name);

  if (it_model == this->block_models.end()) {
    printf("Error : Failed to find block model for full id : \"%s\". Detail : "
           "undefined reference to model named \"%s\".\n",
           block_state_str.c_str(), model.model_name);
    return nullptr;
  }

  return &it_model->second;

  // auto jt=this->block_models.find(it->second.)
}

bool resource_pack::compute_projection(const std::string &block_state_str,
                                       VCL_face_t face_exposed,
                                       block_model::EImgRowMajor_t *const img,
                                       buffer_t &buffer) const noexcept {

  const block_model::model *model =
      this->find_model(block_state_str, face_exposed, &face_exposed, buffer);

  if (model == nullptr) {
    return false;
  }

  model->projection_image(face_exposed, img);
  return true;
}