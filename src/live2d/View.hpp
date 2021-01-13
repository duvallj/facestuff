#ifndef LIVE2D_VIEW_HPP
#define LIVE2D_VIEW_HPP

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <CubismFramework.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

#include "Sprite.hpp"
#include "Model.hpp"
#include <acgl/gui.h>

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
   * @brief After view has been instantiated, initialize it
   */
  void initialize();

  /**
   * @brief Render the view
   * 
   * Renders all objects in the view to the screen
   */
  void render();

  /**
   * @brief Initializes all internal sprites
   */
  void initialize_sprites();

  /**
   * @brief Resizes all sprites to match the new window size
   */
  void resize_sprites();

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

  void pre_model_draw(Model& model_ref);
  void post_model_draw(Model& model_ref);

  /**
   * @brief Get the alpha of the sprite to be drawn
   * 
   * Calculates this based on the model number, for some reason, idk honestly
   */
  float get_sprite_alpha(int model_num) const;

  /**
   * @brief Set the default background color
   * 
   * @param[in] r
   * @param[in] g
   * @param[in] b
   */
  void set_render_target_clear_color(float r, float g, float b);

private:
  Csm::CubismMatrix44* _deviceToScreen;
  Csm::CubismViewMatrix* _viewMatrix;
  GLuint _programId;
  
  ACGL_gui_t* gui;
  Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;
  float _clearColor[4];
};

#endif /* LIVE2D_VIEW_HPP */