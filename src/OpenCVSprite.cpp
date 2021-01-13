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

OpenCVSprite::~OpenCVSprite() {
  // Pass
}

void OpenCVSprite::update(cv::Mat& frame) {
  if (frame.cols != width || frame.rows != height) {
    std::cerr << "Input image is " << frame.cols << " x " << frame.rows \
      << ", but OpenCVSprite is " << width << " x " << height << std::endl;
    return;
  }

  glBindTexture(GL_TEXTURE_2D, _textureId);
  glTexSubImage2D(GL_TEXTURE_2D,
    0, // Mipmap level (0 b/c we are the root)
    GL_BGR, // Pixel format to convert to (same as creation)
    width,
    height,
    0, // Border width
    GL_BGR, // Input OpenCV format
    GL_UNSIGNED_BYTE, // Data type
    frame.ptr() // Pointer to frame's buffer
  );
  glBindTexture(GL_TEXTURE_2D, 0);
}