#include "TextureManager.hpp"
#include <iostream>

#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Util.hpp"
#include "Definitions.hpp"

TextureManager::TextureManager() { }
TextureManager::~TextureManager() {
  release_textures();
}

TextureManager::TextureInfo* TextureManager::create_texture_from_png(std::string filename) {
  // Search for an existing loaded texture with that filename
  for (Csm::csmUint32 i = 0; i < _textures.GetSize(); i++) {
    if (_textures[i]->filename == filename) {
      return _textures[i];
    }
  }

  GLuint texture_id;
  int width, height, channels;
  unsigned int size;
  unsigned char* raw_data;
  unsigned char* png_data;

  raw_data = LAppUtil::load_file_as_bytes(filename, &size);
  if (raw_data == NULL) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error: unable to load texture file %s", filename.c_str());
    }
    return NULL;
  }

  png_data = stbi_load_from_memory(
    raw_data, static_cast<int>(size),
    &width, &height, &channels,
    STBI_rgb_alpha
    );

  if (png_data == NULL) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error: loading texture file %s as PNG failed", filename.c_str());
    }
    LAppUtil::release_bytes(raw_data);
    return NULL;
  }

#ifdef PREMULTIPLIED_ALPHA_ENABLE
  // Pre-multiply the PNG image by its alpha channel
  FULL_COLOR_TYPE* colors = reinterpret_cast<FULL_COLOR_TYPE*>(png_data);
  for (int i = 0; i < width * height; i++) {
    unsigned char* p = png_data + i * 4;
    colors[i] = premultiply(p[0], p[1], p[2], p[3]);
  }
#endif

  // Below code using OpenGL API to load the texture
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png_data);

  // Generate Mipmap (for performant scaling) and set the blending modes
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbind the current texture and release all intermediate data
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(png_data);
  png_data = NULL;
  LAppUtil::release_bytes(raw_data);
  raw_data = NULL;

  // Now, add all requisite infomation into our internal texture list
  TextureManager::TextureInfo* texture_info = new TextureManager::TextureInfo();
  if (texture_info != NULL) {
    texture_info->filename = filename;
    texture_info->width = width;
    texture_info->height = height;
    texture_info->id = texture_id;

    _textures.PushBack(texture_info);
  }
  else if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("Error allocating memory to load texture from %s", filename.c_str());
  }

  return texture_info;
}

TextureManager::TextureInfo* TextureManager::create_texture_from_dims(GLint width, GLint height) {
  if (width == 0 || height == 0) {
    // Invalid width/height was given
    return nullptr;
  }

  GLuint texture_id;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  // These will be reading in OpenCV mats, which have BGR format
  // Having NULL as the final argument makes it allocate a new pixel array that we copy into later
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  // Generate Mipmap (for performant scaling) and set the blending modes
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // These control the method used to clamp values
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, 0);

  TextureManager::TextureInfo* texture_info = new TextureManager::TextureInfo();
  if (texture_info != NULL) {
    texture_info->filename = "";
    texture_info->width = width;
    texture_info->height = height;
    texture_info->id = texture_id;

    _textures.PushBack(texture_info);
  }
  else if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("Error allocating memory for new blank (%d x %d) texture", width, height);
  }

  return texture_info;
}

void TextureManager::release_textures() {
  for (Csm::csmUint32 i = 0; i < _textures.GetSize(); i++) {
    delete _textures[i];
  }

  _textures.Clear();
}

void TextureManager::release_texture(GLuint texture_id) {
  for (Csm::csmUint32 i = 0; i < _textures.GetSize(); i++) {
    if (_textures[i]->id == texture_id) {
      delete _textures[i];
      _textures.Remove(i);
      i--;
    }
  }
}

void TextureManager::release_texture(std::string filename) {
  for (Csm::csmUint32 i = 0; i < _textures.GetSize(); i++) {
    if (_textures[i]->filename == filename) {
      delete _textures[i];
      _textures.Remove(i);
      i--;
    }
  }
}

TextureManager::TextureInfo* TextureManager::get_texture_info_by_id(GLuint texture_id) const {
  for (Csm::csmUint32 i = 0; i < _textures.GetSize(); i++) {
    if (_textures[i]->id == texture_id) {
      return _textures[i];
    }
  }

  // No texture with that id
  return NULL;
}