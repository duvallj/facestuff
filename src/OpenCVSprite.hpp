#ifndef DISPLAY_OPENCV_HPP
#define DISPLAY_OPENCV_HPP

#include <gl/glew.h>
#include <opencv2/core.hpp>
#include <SDL2/SDL.h>

#include "live2d/Sprite.hpp"
#include "live2d/TextureManager.hpp"


class OpenCVSprite : public Sprite {
public:
  const int width;
  const int height;

  /**
   * @brief Custom constructor/destructor
   * 
   * @param[in] texture_manager
   * @param[in] frame_width
   * @param[in] frame_height
   */
  OpenCVSprite(TextureManager* texture_manager, GLuint program_id, const int frame_width, const int frame_height);
  ~OpenCVSprite();

  /**
   * @brief Update this sprite's associated texture with data from the frame
   * 
   * @param[in] frame
   */
  void update(cv::Mat& frame);

  // This inherits from the main Sprite, and so contains it's `render` function
};

#endif /* DISPLAY_OPENCV_HPP */