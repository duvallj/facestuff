#include "Util.hpp"

#include <cstdio>
#include <cstdarg>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <Model/CubismMoc.hpp>
#include "Definitions.hpp"

using std::endl;

double LAppUtil::current_frame = 0.0;
double LAppUtil::last_frame = 0.0;
double LAppUtil::delta_time = 0.0;

Csm::csmByte* LAppUtil::load_file_as_bytes(const std::string filePath, Csm::csmSizeInt* outSize) {
  const char* path = filePath.c_str();

  size_t size = 0;
  struct stat statBuf;
  if (stat(path, &statBuf) == 0) {
    size = statBuf.st_size;
  }
  else {
    // Getting file information failed, note that
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error getting file information for %s", filePath);
    }
    return NULL;
  }

  std::fstream file;
  char* buf = static_cast<char*>(malloc(size));

  file.open(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error opening file %s", filePath);
    }
    return NULL;
  }

  file.read(buf, size);
  file.close();

  if (outSize != NULL) {
    *outSize = static_cast<Csm::csmSizeInt>(size);
  }
  return reinterpret_cast<Csm::csmByte*>(buf);
}

void LAppUtil::release_bytes(Csm::csmByte* byteData) {
  free(byteData);
}

Csm::csmFloat32 LAppUtil::get_delta_time() {
  return static_cast<Csm::csmFloat32>(delta_time);
}

void LAppUtil::update_time() {
  current_frame = glfwGetTime();
  delta_time = current_frame = last_frame;
  last_frame = current_frame;
}

void LAppUtil::print_log(const Csm::csmChar* format, ...) {
  va_list args;
  Csm::csmChar buf[1024];
  va_start(args, format);
  vsnprintf_s(buf, sizeof(buf), format, args);

  std::cerr << buf << std::endl;

  va_end(args);
}