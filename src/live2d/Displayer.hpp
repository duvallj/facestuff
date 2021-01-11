/**
* Code that controls how the Live2D model is rendered to a window
* 
* I should probably break this up into multiple files eventually, but while I'm developing it will stay in one
*/
#ifndef DISPLAYER_HPP
#define DISPLAYER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CubismFramework.hpp>
#include "Allocator.hpp"
#include "TextureManager.hpp"
#include "ShaderManager.hpp"

class Displayer {
public:
  /**
   * @brief Initializer the displayer
   * 
   * @return true iff initialization succeeded
   */
  bool initialize();

  /**
   * @brief Release the current displayer instance
   */
  void release();

  /**
   * @brief Start the mainloop of the current displayer instance
   */
  void run();

  GLFWwindow* get_window() { return _window; }
  TextureManager* get_texture_manager() { return _textureManager; }
  ShaderManager* get_shader_manager() { return _shaderManager; }

  bool get_is_end() { return _isEnd; }
  void app_end() { _isEnd = true; }

private:
  Displayer();

  ~Displayer();

  /**
  * @brief Initialize the Cubism SDK
  */
  void initialize_cubism();

  LAppAllocator _cubismAllocator;
  TextureManager* _textureManager;
  ShaderManager* _shaderManager;
  Csm::CubismFramework::Option _cubismOptions;
  GLFWwindow* _window;
  bool _isEnd;

  int _windowWidth;
  int _windowHeight;
};

#endif /* DISPLAYER_HPP */
