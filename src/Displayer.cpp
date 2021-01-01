#include "Displayer.hpp"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Utils/CubismDebug.hpp>

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

  Csm::CubismFramework::Dispose();
}

void Displayer::run() {
  while (glfwWindowShouldClose(_window) == GL_FALSE) {
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    if ((_windowWidth != width || _windowHeight != height) && width > 0 && height > 0) {
      // TODO: resize sprite here?

      _windowWidth = width;
      _windowHeight = height;
      glViewport(0, 0, width, height);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    // TODO: render view here

    // Update window with new buffer
    glfwSwapBuffers(_window);

    // Process remaining events
    glfwPollEvents();
  }

  release();
}

Displayer::Displayer() :
  _cubismOption(),
  _window(NULL),
  _isEnd(false),
  _windowWidth(0),
  _windowHeight(0)
{
  // pass
}

Displayer::~Displayer() {
  // pass
}

void Displayer::initialize_cubism() {
  _cubismOption.LogFunction = NULL; // change this to LAppPal::PrintMessage
  _cubismOption.LoggingLevel = Live2D::Cubism::Framework::CubismFramework::Option::LogLevel_Verbose; // LAppDefine::CubismLoggingLevel
  Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

  Csm::CubismFramework::Initialize();

  // Initialize rest of view here
}

GLuint Displayer::create_shader()
{
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexShader =
    "#version 120\n"
    "attribute vec3 position;"
    "attribute vec2 uv;"
    "varying vec2 vuv;"
    "void main(void){"
    "    gl_Position = vec4(position, 1.0);"
    "    vuv = uv;"
    "}";
  glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
  glCompileShader(vertexShaderId);
  if (!check_shader(vertexShaderId))
  {
    return 0;
  }

  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentShader =
    "#version 120\n"
    "varying vec2 vuv;"
    "uniform sampler2D texture;"
    "uniform vec4 baseColor;"
    "void main(void){"
    "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
    "}";
  glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
  glCompileShader(fragmentShaderId);
  if (!check_shader(fragmentShaderId))
  {
    return 0;
  }

  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);

  glLinkProgram(programId);

  glUseProgram(programId);

  return programId;
}

bool Displayer::check_shader(GLuint shaderId)
{
  GLint status;
  GLint logLength;
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0)
  {
    GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
    glGetShaderInfoLog(shaderId, logLength, &logLength, log);
    CubismLogError("Shader compile log: %s", log);
    CSM_FREE(log);
  }

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    glDeleteShader(shaderId);
    return false;
  }

  return true;
}