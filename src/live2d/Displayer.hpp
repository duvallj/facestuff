/**
* Code that controls how the Live2D model is rendered to a window
* 
* I should probably break this up into multiple files eventually, but while I'm developing it will stay in one
*/
#ifndef DISPLAYER_HPP
#define DISPLAYER_HPP

#include <GL/glew.h>
#include <SDL.h>
#include <opencv2/core.hpp>
#include <CubismFramework.hpp>
extern "C" {
#include <acgl/gui.h>
#include <acgl/inputhandler.h>
#include <acgl/threads.h>
}
#include "Allocator.hpp"
#include "TextureManager.hpp"
#include "ShaderManager.hpp"
#include "View.hpp"

class Displayer {
public:
  /**
   * @brief Custom constructor/destructor pair
   */
  Displayer();
  ~Displayer();

  /**
   * @brief Initializer the displayer
   * 
   * @return true iff initialization succeeded
   */
  bool initialize(const int cv_width, const int cv_height);

  /**
   * @brief Release the current displayer instance
   */
  void release();

  /**
   * @brief Check if the window size has changed, and if so, refresh the screen
   * 
   * Comes in static and normal variants to interop with C code
   */
  static int check_resize(SDL_Event e, void* obj);
  void check_resize();

  /**
   * @brief Render a frame to the screen
   */
  void render();

  void update_cv(cv::Mat& frame);

  SDL_Window* get_window() const { return _window; }
  TextureManager* get_texture_manager() const { return _textureManager; }
  ShaderManager* get_shader_manager() const { return _shaderManager; }

  bool get_is_end() { return _isEnd; }
  void app_end() { _isEnd = true; }

  /**
   * @brief Static callback for interop with C code
   */
  static int app_end(SDL_Event e, void* obj);

private:

  /**
   * @brief Initialize the Cubism SDK
   */
  void initialize_cubism(const int cv_width, const int cv_height);

  LAppAllocator _cubismAllocator;
  TextureManager* _textureManager;
  ShaderManager* _shaderManager;
  Csm::CubismFramework::Option _cubismOptions;
  SDL_Window* _window;
  SDL_GLContext _context;
  LAppView* _view;
  bool _isEnd;

  int _windowWidth;
  int _windowHeight;
};

#endif /* DISPLAYER_HPP */
