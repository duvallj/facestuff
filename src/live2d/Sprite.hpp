#ifndef LIVE2D_SPRITE_HPP
#define LIVE2D_SPRITE_HPP

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "ShaderManager.hpp"

/**
 * @brief Class in charge of rendering a single 2D Sprite
 */
class Sprite {
public:
  /**
   * @brief Controls where on the screen the sprite is drawn
   * 
   * Each coordinate controls how far away the edge of the sprite is from the corresponding
   * edge of the screen. Larger values for `left` and `up` move it further away, while
   * larger values for `right` and `down` move it closer.
   * 
   * The main reason it's done like this is to make OpenGL position vertex calculations
   * easier once the `x`, `y`, `width`, and `height` have been read in.
   */
  struct Rect {
  public:
    float left;
    float right;
    float top;
    float bottom;
  };

  /**
   * @brief Construct a new sprite
   * 
   * @param[in] x
   * @param[in] y
   * @param[in] width
   * @param[in] height
   * @param[in] texture_id The initialized texture the sprite will render
   * @param[in] program_id The initialized shader program that will render the sprite
   */
  Sprite(float x, float y, float width, float height, GLuint texture_id, GLuint program_id);

  /**
   * @brief Corresponding destructor to the custon constructor
   */
  ~Sprite();

  /**
   * @brief Get the texture used to make this sprite
   * 
   * @return The `texture_id` this sprite was initialized with
   */
  GLuint get_texture_id() { return _textureId; }

  /**
   * @brief Render the sprite using internal data
   * 
   * @param[in] window The window to render to
   */
  void render(GLFWwindow* window) const;

  /**
   * @brief Render the sprite using external texture/uv data
   * 
   * @param[in] window
   * @param[in] texture_id
   * @param[in] uv_vertex
   */
  void render(GLFWwindow* window, GLuint texture_id, const GLfloat uv_vertex[8]) const;

  /**
   * @brief Check for collision with sprite
   */
  bool is_hit(GLFWwindow* window, float x, float y) const;

  /**
   * @brief Change sprite color filter
   * 
   * All input parameters should be in the range 0.0~1.0
   * 
   * @param[in] r
   * @param[in] g
   * @param[in] b
   * @param[in] a
   */
  void set_color(float r, float g, float b, float a);

  /**
   * @brief Change the position of the sprite
   * 
   * @param[in] x
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  void set_position(float x, float y, float width, float height);

private:
  GLuint _textureId;
  Rect _rect;
  GLint _positionLocation;
  GLint _uvLocation;
  GLint _textureLocation;
  GLint _colorLocation;

  float _spriteColor[4];
};

#endif /* LIVE2D_SPRITE_HPP */