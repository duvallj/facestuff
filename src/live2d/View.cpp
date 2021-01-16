#include "View.hpp"
extern "C" {
#include <acgl/contracts.h>
}
#include "Definitions.hpp"

#include <math.h>
#include <string>
#include <iostream>


LAppView::LAppView() :
  _programId(0),
  _gui(NULL),
  _cv_output(NULL),
  _cv_output_node(NULL),
  _model_node(NULL),
  _model(NULL)
{
  _deviceToScreen = new Csm::CubismMatrix44();
  _viewMatrix = new Csm::CubismViewMatrix();

  _clearColor[0] = 1.0f;
  _clearColor[1] = 1.0f;
  _clearColor[2] = 1.0f;
  _clearColor[3] = 0.0f;
}

LAppView::~LAppView() {
  delete _deviceToScreen;
  _deviceToScreen = NULL;
  delete _viewMatrix;
  _viewMatrix = NULL;

  if (_gui != NULL) {
    ACGL_gui_destroy(_gui);
    _gui = NULL;
  }

  if (_cv_output != NULL) {
    delete _cv_output;
    _cv_output = NULL;
  }

  if (_model != NULL) {
    delete _model;
    _model = NULL;
  }
}

void LAppView::initialize_matricies(SDL_Window* window) {
  REQUIRES(window != NULL);
  int width, height;
  SDL_GL_GetDrawableSize(window, &width, &height);

  if (width == 0 || height == 0) {
    return;
  }

  if (_gui == NULL) {
    _gui = ACGL_gui_init(window);
  }

  float ratio = static_cast<float>(width) / static_cast<float>(height);
  float left = -ratio;
  float right = ratio;
  float bottom = LAppDefinitions::ViewLogicalBottom;
  float top = LAppDefinitions::ViewLogicalTop;

  _viewMatrix->SetScreenRect(left, right, bottom, top);
  _viewMatrix->Scale(LAppDefinitions::ViewScale, LAppDefinitions::ViewScale);

  _viewMatrix->SetMinScale(LAppDefinitions::ViewMinScale);
  _viewMatrix->SetMaxScale(LAppDefinitions::ViewMaxScale);

  _viewMatrix->SetMaxScreenRect(
    LAppDefinitions::ViewLogicalMaxLeft,
    LAppDefinitions::ViewLogicalMaxRight,
    LAppDefinitions::ViewLogicalMaxBottom,
    LAppDefinitions::ViewLogicalMaxTop
  );

  _deviceToScreen->LoadIdentity();
  if (width > height) {
    float screen_width = fabsf(right - left);
    _deviceToScreen->ScaleRelative(screen_width / width, -screen_width / width);
  }
  else {
    float screen_height = fabsf(top - bottom);
    _deviceToScreen->ScaleRelative(screen_height / height, -screen_height / height);
  }
  _deviceToScreen->ScaleRelative(-width * 0.5f, -height * 0.5f);
}

void LAppView::initialize_sprites(TextureManager* texture_manager, ShaderManager* shader_manager, const int cv_width, const int cv_height) {
  _programId = shader_manager->create_shader();

  if (_cv_output != NULL) { delete _cv_output; }
  std::string background_path = LAppDefinitions::ResourcesPath;
  background_path = background_path + "/" + LAppDefinitions::BackImageName;
  TextureManager::TextureInfo* texture_info = texture_manager->create_texture_from_png(background_path);
  // _cv_output = new Sprite(texture_info->id, _programId);
  _cv_output = new OpenCVSprite(texture_manager, _programId, cv_width, cv_height);
  // _cv_output = new OpenCVSprite(texture_info->id, _programId, cv_width, cv_height);
  if (_cv_output == NULL) { return; }
  
  if (_cv_output_node != NULL) { ACGL_gui_node_destroy(_cv_output_node); }
  _cv_output_node = ACGL_gui_node_init(
    _gui,
    OpenCVSprite::render,
    NULL,
    reinterpret_cast<void*>(_cv_output)
  );
  if (_cv_output_node == NULL) { return; }

  std::string model_path = LAppDefinitions::ModelDir[0];
  model_path = LAppDefinitions::ResourcesPath + model_path + "/";
  std::string model_json = LAppDefinitions::ModelDir[0];
  model_json = model_json + ".model3.json";

  if (_model != NULL) {
    delete _model;
  }
  _model = new Model();
  if (_model == NULL) { return; }
  _model->load_assets(texture_manager, model_path.c_str(), model_json.c_str());

  if (_model_node != NULL) {
    ACGL_gui_node_destroy(_model_node);
  }
  _model_node = ACGL_gui_node_init(
    _gui,
    LAppView::render_model,
    NULL,
    reinterpret_cast<void*>(this)
  );
  if (_model_node == NULL) { return; }
  _model_node->node_type = ACGL_GUI_NODE_FILL_H | ACGL_GUI_NODE_PRESERVE_ASPECT;
  _model_node->w = 1;
  _model_node->h = 1;
  _model_node->w_frac = 1;
  _model_node->h_frac = 1;

  ACGL_gui_node_remove_all_children(_gui->root);
  ACGL_gui_node_add_child_front(_gui->root, _cv_output_node);
  ACGL_gui_node_add_child_front(_gui->root, _model_node);
}

void LAppView::update_cv(cv::Mat& frame) {
  _cv_output->update(frame);
  if (SDL_LockMutex(_cv_output_node->mutex) != 0) {
    fprintf(stderr, "Error, could not lock _cv_output_node->mutex in LAppView::update_cv: %s\n", SDL_GetError());
    return;
  }
  _cv_output_node->needs_update = true;
  SDL_UnlockMutex(_cv_output_node->mutex);
}

bool LAppView::render_model(SDL_Window* window, SDL_Rect area, void* obj) {
  LAppView* view = reinterpret_cast<LAppView*>(obj);
  return view->render_model(window, area);
}

bool LAppView::render_model(SDL_Window* window, SDL_Rect area) {
  float x = static_cast<float>(area.x);
  float y = static_cast<float>(area.y);
  float w = static_cast<float>(area.w);
  float h = static_cast<float>(area.h);

  int screen_width, screen_height;
  SDL_GL_GetDrawableSize(window, &screen_width, &screen_height);
  float sw = static_cast<float>(screen_width);
  float sh = static_cast<float>(screen_height);

  Csm::CubismMatrix44 projection;

  std::cout << "Rendering model" << std::endl;
  std::cout << "x:" << x << ", y:" << x << std::endl;
  projection.Translate(x / screen_width, -y / screen_height);
  std::cout << "w:" << w << ", sw:" << sw << ", h:" << h << ", sh:" << sh << std::endl;
  //projection.Scale(width / screen_width, height / screen_height);

  if (_viewMatrix != NULL) {
    projection.MultiplyByMatrix(_viewMatrix);
  }

  _model->update();
  _model->draw(projection);

  return true;
}

void LAppView::render() {
  std::cerr << "OpenGL Error Before View::render: " << gluErrorString(glGetError()) << std::endl;
  // Always forcing updates b/c GL swaps framebuffers always, nothing is persistent
  ACGL_gui_force_update(_gui);
  ACGL_gui_render(_gui);
  std::cerr << "OpenGL Error After View::render: " << gluErrorString(glGetError()) << std::endl;
}

float LAppView::device_to_view_x(float device_x) const {
  float screen_x = device_to_screen_x(device_x);
  return _viewMatrix->InvertTransformX(screen_x);
}

float LAppView::device_to_view_y(float device_y) const {
  float screen_y = device_to_screen_y(device_y);
  return _viewMatrix->InvertTransformY(screen_y);
}

float LAppView::device_to_screen_x(float device_x) const {
  return _deviceToScreen->TransformX(device_x);
}

float LAppView::device_to_screen_y(float device_y) const {
  return _deviceToScreen->TransformY(device_y);
}

void LAppView::set_render_target_clear_color(float r, float g, float b) {
  _clearColor[0] = r;
  _clearColor[1] = g;
  _clearColor[2] = b;
}