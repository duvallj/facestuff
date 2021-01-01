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
#include "live2d/Allocator.hpp"

class Displayer {
public:
  bool initialize();
  void release();
  void run();

  GLuint create_shader();

  GLFWwindow* GetWindow() { return _window; }

  bool get_is_end() { return _isEnd; }

  void app_end() { _isEnd = true; }

private:
  Displayer();

  ~Displayer();

  /**
  * @brief Initialize the Cubism SDK
  */
  void initialize_cubism();

  bool check_shader(GLuint shaderId);

  LAppAllocator _cubismAllocator;
  Csm::CubismFramework::Option _cubismOption;
  GLFWwindow* _window;
  bool _isEnd;

  int _windowWidth;
  int _windowHeight;
};

#endif /* DISPLAYER_HPP */
