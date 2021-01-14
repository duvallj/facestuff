#ifndef LIVE2D_VIEW_HPP
#define LIVE2D_VIEW_HPP

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <CubismFramework.hpp>

#include "Sprite.hpp"
#include "Model.hpp"

#include "../OpenCVSprite.hpp"
extern "C" {
#include <acgl/gui.h>
}

/**
 * This class is in charge of instantiating background sprites and models, resizing them appropriately, and
 * rendering them to the screen.
 */
class LAppView {
public:
  /**
   * @brief Custom constructor/destructor
   */
  LAppView();
  ~LAppView();

  /**
   * @brief Re-initialize view matricies from the window height/width
   * 
   * Must be called when the window resizes
   */
  void initialize_matricies(SDL_Window* window);

  /**
   * @brief Initializes all internal sprites
   */
  void initialize_sprites(TextureManager* texture_manager, ShaderManager* shader_manager, const int cv_width, const int cv_height);

  /**
   * @brief static callback to render just the Live2D model
   */
  static bool render_model(SDL_Window* window, SDL_Rect area, void* obj);
  bool render_model(SDL_Window* window, SDL_Rect area);

  /**
 * @brief Render the view
 *
 * Renders all objects in the view to the screen
 */
  void render();

  /**
   * @brief Convert device coordinate to coordinate in the view
   * 
   * @param[in] device_x
   * @return The corresponding x coordinate in the view
   */
  float device_to_view_x(float device_x) const;

  /**
   * @brief Convert device coordinate to coordinate in the view
   *
   * @param[in] device_y
   * @return The corresponding y coordinate in the view
   */
  float device_to_view_y(float device_y) const;

  /**
   * @brief Convert device coordinate to coordinate in the internal screen
   *
   * @param[in] device_x
   * @return The corresponding x coordinate in the view
   */
  float device_to_screen_x(float device_x) const;

  /**
   * @brief Convert device coordinate to coordinate in the internal screen
   *
   * @param[in] device_y
   * @return The corresponding y coordinate in the view
   */
  float device_to_screen_y(float device_y) const;

  /**
   * @brief Set the default background color
   * 
   * @param[in] r
   * @param[in] g
   * @param[in] b
   */
  void set_render_target_clear_color(float r, float g, float b);

  /**
   * @brief Update the internal OpenCV sprite
   */
  void update_cv(cv::Mat& frame);

  ACGL_gui_t* get_gui() const { return _gui; }

private:
  Csm::CubismMatrix44* _deviceToScreen;
  Csm::CubismViewMatrix* _viewMatrix;
  GLuint _programId;

  Sprite* _cv_output;
  ACGL_gui_object_t* _cv_output_node;

  Model* _model;
  ACGL_gui_object_t* _model_node;
  
  ACGL_gui_t* _gui;
  Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;
  float _clearColor[4];
};

#endif /* LIVE2D_VIEW_HPP */