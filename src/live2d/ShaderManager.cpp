#include "ShaderManager.hpp"

GLuint ShaderManager::create_shader()
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
  if (!check_shader(vertexShaderId)) {
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
  if (!check_shader(fragmentShaderId)) {
    return 0;
  }

  GLuint programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);

  glLinkProgram(programId);
  // It's safe to delete these shaders once they have been linked
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  if (!check_program(programId)) {
    return 0;
  }

  glUseProgram(programId);
  _programs.PushBack(programId);

  return programId;
}

bool ShaderManager::check_shader(GLuint shaderId)
{
  GLint status;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
      glGetShaderInfoLog(shaderId, logLength, NULL, log);
      CubismLogError("Shader compile log: %s", log);
      CSM_FREE(log);
    }

    glDeleteShader(shaderId);
    return false;
  }

  return true;
}

bool ShaderManager::check_program(GLuint programId) {
  GLint status;
  glGetProgramiv(programId, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    GLint logLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
    glGetProgramInfoLog(programId, logLength, NULL, log);
    CubismLogError("Shader link log: %s", log);
    CSM_FREE(log);

    glDeleteProgram(programId);
    return false;
  }

  return true;
}

ShaderManager::ShaderManager()
  : _programs()
{
  // Pass
}

ShaderManager::~ShaderManager() {
  release_shaders();
}

void ShaderManager::release_shaders() {
  for (Csm::csmUint32 i = 0; i < _programs.GetSize(); i++) {
    glDeleteProgram(_programs[i]);
  }

  _programs.Clear();
}