#include "Displayer.hpp"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Utils/CubismDebug.hpp>

#include "Util.hpp"
#include "Definitions.hpp"

static const int DEFAULT_WIDTH = 640;
static const int DEFAULT_HEIGHT = 480;

bool Displayer::initialize() {
  if (glfwInit() == GL_FALSE) {
    std::cout << "Error initializing GLFW" << std::endl;
    return GL_FALSE;
  }

  _window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "SAMPLE", NULL, NULL);
  if (_window == NULL) {
    std::cout << "Error creating GLFW window" << std::endl;
    glfwTerminate();
    return GL_FALSE;
  }

  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW" << std::endl;
    glfwTerminate();
    return GL_FALSE;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glfwGetWindowSize(_window, &_windowWidth, &_windowHeight);

  initialize_cubism();

  return GL_TRUE;
}

void Displayer::release() {
  glfwDestroyWindow(_window);
  glfwTerminate();

  delete _textureManager;

  Csm::CubismFramework::Dispose();
}

void Displayer::run() {
  while (glfwWindowShouldClose(_window) == GL_FALSE && !_isEnd) {
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    if ((_windowWidth != width || _windowHeight != height) && width > 0 && height > 0) {
      // TODO: resize sprite here?

      _windowWidth = width;
      _windowHeight = height;
      glViewport(0, 0, width, height);
    }

    LAppUtil::update_time();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    // TODO: render view here

    // Update window with new buffer
    glfwSwapBuffers(_window);

    // Process remaining events
    glfwPollEvents();
  }
}

Displayer::Displayer() :
  _cubismOptions(),
  _window(NULL),
  _isEnd(false),
  _windowWidth(0),
  _windowHeight(0)
{
  _textureManager = new TextureManager();
}

Displayer::~Displayer() {
  release();
}

void Displayer::initialize_cubism() {
  _cubismOptions.LogFunction = LAppUtil::print_message;
  _cubismOptions.LoggingLevel = LAppDefinitions::CubismLoggingLevel; // Live2D::Cubism::Framework::CubismFramework::Option::LogLevel::LogLevel_Verbose;
  
  Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOptions);
  Csm::CubismFramework::Initialize();

  // Initialize rest of view here

  LAppUtil::update_time();
}