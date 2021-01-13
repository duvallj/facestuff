#ifndef LIVE2D_TEXTURE_MANAGER_HPP
#define LIVE2D_TEXTURE_MANAGER_HPP

#include <string>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <Type/csmVector.hpp>

typedef uint32_t FULL_COLOR_TYPE;
typedef uint8_t PART_COLOR_TYPE;

class TextureManager {
public:
  struct TextureInfo {
    GLuint id = 0;
    int width = -1;
    int height = -1;
    std::string filename;
  };

  /**
   * @brief Custom constructors/destructors
   * 
   * We have a custom destructor, so style guides mandate we provide a custom
   * constructor as well.
   */
  TextureManager();
  ~TextureManager();

  /**
   * @brief Blend a single color part by an alpha transparency
   * 
   * @param[in] channel The channel to blend
   * @param[in] alpha The amount to blend by
   * 
   * @return The channel modulated by the alpha amount
   */
  inline FULL_COLOR_TYPE alpha_blend(PART_COLOR_TYPE channel, PART_COLOR_TYPE alpha) {
    FULL_COLOR_TYPE c, a;
    c = static_cast<FULL_COLOR_TYPE>(channel);
    a = static_cast<FULL_COLOR_TYPE>(alpha);

    return (c * (a + 1)) >> 8;
  }

  /**
   * @brief Pack values into single ARGB integer
   * 
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   * @param[in] alpha
   * 
   * @return A packed ARGB with the RGB channels modulated by alpha
   */
  inline FULL_COLOR_TYPE premultiply(PART_COLOR_TYPE red, PART_COLOR_TYPE green, PART_COLOR_TYPE blue, PART_COLOR_TYPE alpha) {
    return \
      alpha_blend(red, alpha) | \
      (alpha_blend(green, alpha) << 8) | \
      (alpha_blend(blue, alpha) << 16) | \
      (static_cast<FULL_COLOR_TYPE>(alpha) << 24);
  }

  /**
   * @brief Given a PNG, create a GL texture from it
   * 
   * @param[in] filename
   */
  TextureInfo* create_texture_from_png(std::string filename);

  /**
   * @brief Allocate space for a texture of the specified dimensions
   * 
   * @param[in] width
   * @param[in] height
   */
  TextureInfo* create_texture_from_dims(GLint width, GLint height);

  /**
   * @brief Release all created textures
   */
  void release_textures();

  /**
   * @brief Release a specific texture by ID
   * 
   * @param[in] texture_id
   */
  void release_texture(GLuint texture_id);

  /**
   * @brief Release a specific texture by loaded filename
   * 
   * @param[in] filename
   */
  void release_texture(std::string filename);

  /**
   * @brief Re-fetch a loaded texture using its ID
   * 
   * @param[in] texture_id
   * @return The TextureInfo object corresponding to that ID
   */
  TextureInfo* get_texture_info_by_id(GLuint texture_id) const;

private:
  Csm::csmVector<TextureInfo*> _textures;
};

#endif /* LIVE2D_TEXTURE_MANAGER_HPP */