#include "Displayer.hpp"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Utils/CubismDebug.hpp>

#include "Util.hpp"
#include "Definitions.hpp"

static const char* DEFAULT_NAME = "FaceStuff";
static const int DEFAULT_WIDTH = 640;
static const int DEFAULT_HEIGHT = 480;

bool Displayer::initialize() {
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

  if (glewInit() != GLEW_OK) {
    std::cerr << "Error initializing GLEW" << std::endl;
    SDL_DestroyWindow(_window);
    SDL_Quit();
    return false;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SDL_GL_GetDrawableSize(_window, &_windowWidth, &_windowHeight);

  initialize_cubism();

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

void Displayer::check_resize(SDL_Event, void* obj) {
  Displayer* disp = reinterpret_cast<Displayer*>(obj);
  disp->check_resize();
}

void Displayer::check_resize() {
  int width, height;
  SDL_GL_GetDrawableSize(_window, &width, &height);
  if ((_windowWidth != width || _windowHeight != height) && width > 0 && height > 0) {
    _windowWidth = width;
    _windowHeight = height;
    glViewport(0, 0, width, height);

    render();
  }
}

void Displayer::render(SDL_Event, void* obj) {
  Displayer* disp = reinterpret_cast<Displayer*>(obj);
  disp->render();
}

void Displayer::render() {
  _view->render();
}

Displayer::Displayer() :
  _cubismOptions(),
  _window(NULL),
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

void Displayer::initialize_cubism() {
  _cubismOptions.LogFunction = LAppUtil::print_message;
  _cubismOptions.LoggingLevel = LAppDefinitions::CubismLoggingLevel; // Live2D::Cubism::Framework::CubismFramework::Option::LogLevel::LogLevel_Verbose;
  
  Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOptions);
  Csm::CubismFramework::Initialize();

  // TODO: Initialize rest of view here

  LAppUtil::update_time();
}