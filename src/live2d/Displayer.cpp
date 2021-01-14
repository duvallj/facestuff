#include "Displayer.hpp"

#include <iostream>
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <Utils/CubismDebug.hpp>

#include "Util.hpp"
#include "Definitions.hpp"

static const char* DEFAULT_NAME = "FaceStuff";
static const int DEFAULT_WIDTH = 640;
static const int DEFAULT_HEIGHT = 480;

bool Displayer::initialize(const int cv_width, const int cv_height) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return false;
  }

  _window = SDL_CreateWindow(
    DEFAULT_NAME,
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    DEFAULT_WIDTH, DEFAULT_HEIGHT,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL
  );
  if (_window == NULL) {
    fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  // Create the OpenGL context
  _context = SDL_GL_CreateContext(_window);
  if (_context == NULL) {
    fprintf(stderr, "Error initializing GL context: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(glewError));
    SDL_DestroyWindow(_window);
    SDL_Quit();
    return false;
  }

  // Set VSync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    fprintf(stderr, "Warning: unable to set VSync: %s\n", SDL_GetError());
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SDL_GL_GetDrawableSize(_window, &_windowWidth, &_windowHeight);

  initialize_cubism(cv_width, cv_height);

  return true;
}

void Displayer::release() {
  SDL_DestroyWindow(_window);
  SDL_Quit();

  delete _textureManager;
  delete _shaderManager;
  delete _view;

  Csm::CubismFramework::Dispose();
}

int Displayer::check_resize(SDL_Event, void* obj) {
  Displayer* disp = reinterpret_cast<Displayer*>(obj);
  disp->check_resize();

  return 0;
}

void Displayer::check_resize() {
  int width, height;
  SDL_GL_GetDrawableSize(_window, &width, &height);
  if ((_windowWidth != width || _windowHeight != height) && width > 0 && height > 0) {
    _windowWidth = width;
    _windowHeight = height;
    glViewport(0, 0, width, height);

    _view->initialize_matricies(_window);

    render();
  }
}

void Displayer::render() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearDepth(1.0);

  _view->render();

  SDL_GL_SwapWindow(_window);
}

Displayer::Displayer() :
  _cubismOptions(),
  _window(NULL),
  _context(NULL),
  _isEnd(false),
  _windowWidth(0),
  _windowHeight(0)
{
  _textureManager = new TextureManager();
  _shaderManager = new ShaderManager();
  _view = new LAppView();
}

Displayer::~Displayer() {
  release();
}

void Displayer::initialize_cubism(const int cv_width, const int cv_height) {
  _cubismOptions.LogFunction = LAppUtil::print_message;
  _cubismOptions.LoggingLevel = LAppDefinitions::CubismLoggingLevel; // Live2D::Cubism::Framework::CubismFramework::Option::LogLevel::LogLevel_Verbose;
  
  Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOptions);
  Csm::CubismFramework::Initialize();

  _view->initialize_matricies(_window);
  _view->initialize_sprites(_textureManager, _shaderManager, cv_width, cv_height);

  LAppUtil::update_time();
}

void Displayer::update_cv(cv::Mat& frame) {
  _view->update_cv(frame);
}

int Displayer::app_end(SDL_Event e, void* obj) {
  Displayer* disp = reinterpret_cast<Displayer*>(obj);
  disp->app_end();
  return 0;
}