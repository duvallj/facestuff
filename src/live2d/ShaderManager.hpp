#ifndef LIVE2D_SHADER_MANAGER
#define LIVE2D_SHADER_MANAGER

#include <gl/glew.h>
#include <CubismFramework.hpp>
#include <Type/csmVector.hpp>

/**
 * @brief Class that manages simple shaders used by OpenGL to draw sprites
 */
class ShaderManager {
public:
  /**
   * @brief Custom constructor/destructor pair
   */
  ShaderManager();
  ~ShaderManager();

  /**
   * @brief Creates a new shader
   */
  GLuint create_shader();
  /**
   * @brief Releases all shaders created by this instance
   */
  void release_shaders();

private:
  bool check_shader(GLuint shaderId);
  bool check_program(GLuint programId);
  Csm::csmVector<GLuint> _programs;
};

#endif /* LIVE2D_SHADER_MANAGER */