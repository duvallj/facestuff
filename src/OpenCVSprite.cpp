#include "OpenCVSprite.hpp"
#include <iostream>

OpenCVSprite::OpenCVSprite(TextureManager* texture_manager, GLuint program_id, const int frame_width, const int frame_height)
  : width(frame_width),
    height(frame_height),
    Sprite(0, program_id)
{
  TextureManager::TextureInfo* texture_info = texture_manager->create_texture_from_dims(width, height);
  _textureId = texture_info->id;
}

OpenCVSprite::OpenCVSprite(GLuint texture_id, GLuint program_id, const int frame_width, const int frame_height)
  : width(frame_width),
  height(frame_height),
  Sprite(texture_id, program_id)
{
  // Pass, everything already taken care of by Sprite constructor
}

OpenCVSprite::~OpenCVSprite() {
  // Pass
}

void OpenCVSprite::update(cv::Mat& frame) {
  if (frame.cols != width || frame.rows != height) {
    std::cerr << "Input image is " << frame.cols << " x " << frame.rows \
      << ", but OpenCVSprite is " << width << " x " << height << std::endl;
    return;
  }
  std::cerr << "OpenGL Error Before OpenCVSprite::update: " << gluErrorString(glGetError()) << std::endl;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glTexSubImage2D(GL_TEXTURE_2D,
    0, // Mipmap level (0 b/c we are the root)
    0, // x offset
    0, // y offset
    width,
    height,
    GL_BGR, // Input OpenCV format
    GL_UNSIGNED_BYTE, // Input OpenCV data type
    frame.data // Pointer to frame's buffer
  );
  glBindTexture(GL_TEXTURE_2D, 0);
  std::cerr << "OpenGL Error After OpenCVSprite::update: " << gluErrorString(glGetError()) << std::endl;
}