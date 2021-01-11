#include "Sprite.hpp"
#include "ShaderManager.hpp"

Sprite::Sprite(float x, float y, float width, float height, GLuint texture_id, GLuint program_id) 
: _rect() 
{
  float halfwidth = width * 0.5f;
  float halfheight = height * 0.5f;
  _rect.left = x - halfwidth;
  _rect.right = x + halfwidth;
  _rect.top = y + halfheight;
  _rect.bottom = y - halfheight;

  _textureId = texture_id;

  // Fetch shader attributes from OpenGL
  _positionLocation = glGetAttribLocation(program_id, "position");
  _uvLocation = glGetAttribLocation(program_id, "uv");
  _textureLocation = glGetAttribLocation(program_id, "texture");
  _colorLocation = glGetAttribLocation(program_id, "baseColor");

  _spriteColor[0] = 1.0f;
  _spriteColor[1] = 1.0f;
  _spriteColor[2] = 1.0f;
  _spriteColor[3] = 1.0f;
}

Sprite::~Sprite() {
  // Texture and shader are managed elsewhere, no need to manually free
}

void Sprite::render(GLFWwindow* window) const {
  static const GLfloat uv_vertex_default[8] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
  };

  render(window, _textureId, uv_vertex_default);
}

void Sprite::render(GLFWwindow* window, GLuint texture_id, const GLfloat uv_vertex[8]) const {
  int max_width, max_height;
  glfwGetWindowSize(window, &max_width, &max_height);

  if (max_width == 0 || max_height == 0) {
    return;
  }

  glEnable(GL_TEXTURE_2D);
  glEnableVertexAttribArray(_positionLocation);
  glEnableVertexAttribArray(_uvLocation);
  glUniform1i(_textureLocation, 0);

  // **magic**
  float half_width = max_width * 0.5f;
  float half_height = max_height * 0.5f;
  float position_vertex[8] = {
    (_rect.right - half_width) / half_width, (_rect.top - half_height) / half_height,
    (_rect.left - half_width) / half_width, (_rect.top - half_height) / half_height,
    (_rect.left - half_width) / half_width, (_rect.bottom - half_height) / half_height,
    (_rect.right - half_width) / half_width, (_rect.bottom - half_height) / half_height,
  };

  // Set the vertex paramaters from our computations
  glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, GL_FALSE, 0, position_vertex);
  glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, GL_FALSE, 0, uv_vertex);
  glUniform4f(_colorLocation, _spriteColor[0], _spriteColor[1], _spriteColor[2], _spriteColor[3]);

  // Finally, draw the texture
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

bool Sprite::is_hit(GLFWwindow* window, float x, float y) const {
  int max_width, max_height;
  glfwGetWindowSize(window, &max_width, &max_height);

  if (max_width == 0 || max_height == 0) {
    return false;
  }

  y = max_height - y;

  return x >= _rect.left && x <= _rect.right && y <= _rect.top && y >= _rect.bottom;
}

void Sprite::set_color(float r, float g, float b, float a) {
  _spriteColor[0] = r;
  _spriteColor[1] = g;
  _spriteColor[2] = b;
  _spriteColor[3] = a;
}

void Sprite::set_position(float x, float y, float width, float height) {
  float halfwidth = width * 0.5f;
  float halfheight = height * 0.5f;
  _rect.left = x - halfwidth;
  _rect.right = x + halfwidth;
  _rect.top = y + halfheight;
  _rect.bottom = y - halfheight;
}