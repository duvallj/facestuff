/**
* Code that controls how the Live2D model is rendered to a window
* 
* I should probably break this up into multiple files eventually, but while I'm developing it will stay in one
*/
#ifndef DISPLAYER_HPP
#define DISPLAYER_HPP

#include <GL/glew.h>
#include <SDL.h>
#include <CubismFramework.hpp>
#include <acgl/gui.h>
#include <acgl/inputhandler.h>
#include <acgl/threads.h>
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
  bool initialize();

  /**
   * @brief Release the current displayer instance
   */
  void release();

  /**
   * @brief Check if the window size has changed, and if so, refresh the screen
   * 
   * Comes in static and normal variants to interop with C code
   */
  static void check_resize(SDL_Event e, void* obj);
  void check_resize();

  /**
   * @brief Render a frame to the screen
   */
  static void render(SDL_Event e, void* obj);
  void render();

  SDL_Window* get_window() { return _window; }
  TextureManager* get_texture_manager() { return _textureManager; }
  ShaderManager* get_shader_manager() { return _shaderManager; }

  bool get_is_end() { return _isEnd; }
  void app_end() { _isEnd = true; }

  enum UserEvents {
    RefreshRequest,
  };

private:

  /**
  * @brief Initialize the Cubism SDK
  */
  void initialize_cubism();

  LAppAllocator _cubismAllocator;
  TextureManager* _textureManager;
  ShaderManager* _shaderManager;
  Csm::CubismFramework::Option _cubismOptions;
  SDL_Window* _window;
  LAppView* _view;
  bool _isEnd;

  int _windowWidth;
  int _windowHeight;
};

/**
 * @brief Class that contains all the mainloop logic
 * 
 * WARNING: the code for this is actually contained in Main.cpp
 */
class MainThread {
public:
  static bool setup(void* display);
  static bool loop(void* display);
  static bool cleanup(void* display);
};

#endif /* DISPLAYER_HPP */
