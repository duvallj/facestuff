#include "Sprite.hpp"
#include "Definitions.hpp"
#include "Util.hpp"

Sprite::Sprite(GLuint texture_id, GLuint program_id) 
  : _rect(),
    _lastRect()
{
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

bool Sprite::render(SDL_Window* window, SDL_Rect area, void* obj) {
  Sprite* sp = reinterpret_cast<Sprite*>(obj);
  return sp->render(window, area);
}

bool Sprite::render(SDL_Window* window, SDL_Rect area) {
  static const GLfloat uv_vertex_default[8] = {
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
  };

  if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("Rendering sprite at x:%d y:%d w:%d h:%d", area.x, area.y, area.w, area.h);
  }

  return render_immediate(window, area, _textureId, uv_vertex_default);
}

bool Sprite::render_immediate(SDL_Window* window, SDL_Rect area, GLuint texture_id, const GLfloat uv_vertex[8]) {
  int max_width, max_height;
  SDL_GL_GetDrawableSize(window, &max_width, &max_height);

  if (max_width == 0 || max_height == 0) {
    return false;
  }

  glEnable(GL_TEXTURE_2D);
  glEnableVertexAttribArray(_positionLocation);
  glEnableVertexAttribArray(_uvLocation);
  glUniform1i(_textureLocation, 0);

  // Only recalculate internal _rect if the position has changed
  if (!equals_last_rect(area)) {
    _lastRect = area;
    _rect = Sprite::transform_rect(area);
  }

  // **magic**
  float half_width = max_width * 0.5f;
  float half_height = max_height * 0.5f;
  /* float position_vertex[8] = {
    (_rect.right - half_width) / half_width, (_rect.top - half_height) / half_height,
    (_rect.left - half_width) / half_width, (_rect.top - half_height) / half_height,
    (_rect.left - half_width) / half_width, (_rect.bottom - half_height) / half_height,
    (_rect.right - half_width) / half_width, (_rect.bottom - half_height) / half_height,
  };*/
  float position_vertex[8] = {
    0.538947, 0.950000,
    -0.538947, 0.950000,
    -0.538947, -0.950000,
    0.538947, -0.950000
  };

  fprintf(stderr, "(%f, %f), (%f, %f), (%f, %f), (%f, %f)\n", position_vertex[0], position_vertex[1], position_vertex[2], position_vertex[3], position_vertex[4], position_vertex[5], position_vertex[6], position_vertex[7]);

  // Set the vertex paramaters from our computations
  glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, GL_FALSE, 0, position_vertex);
  glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, GL_FALSE, 0, uv_vertex);
  glUniform4f(_colorLocation, _spriteColor[0], _spriteColor[1], _spriteColor[2], _spriteColor[3]);

  // Finally, draw the texture
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  return true;
}

bool Sprite::is_hit(SDL_Window* window, float x, float y) const {
  int max_width, max_height;
  SDL_GL_GetDrawableSize(window, &max_width, &max_height);

  if (max_width == 0 || max_height == 0) {
    return false;
  }

  y = max_height - y;

  return x >= _rect.left && x <= _rect.right && \
    y <= _rect.top && y >= _rect.bottom;
}

void Sprite::set_color(float r, float g, float b, float a) {
  _spriteColor[0] = r;
  _spriteColor[1] = g;
  _spriteColor[2] = b;
  _spriteColor[3] = a;
}

Sprite::Rect Sprite::transform_rect(SDL_Rect area) {
  Sprite::Rect rect;

  float halfwidth = area.w * 0.5f;
  float halfheight = area.h * 0.5f;
  rect.left = area.x - halfwidth;
  rect.right = area.x + halfwidth;
  rect.top = area.y + halfheight;
  rect.bottom = area.y - halfheight;

  return rect;
}

bool Sprite::equals_last_rect(SDL_Rect area) const {
  return area.x == _lastRect.x && \
    area.y == _lastRect.y && \
    area.w == _lastRect.w && \
    area.h == _lastRect.h;
}