#ifndef LIVE2D_UTIL_HPP
#define LIVE2D_UTIL_HPP

#include <CubismFramework.hpp>
#include <string>

class LAppUtil {
public:
  /**
   * @brief Load the contents of a file into memory
   * 
   * @param[in] filePath
   * @param[out] outSize. If NULL, this parameter is ignored
   * @return A pointer of *outSize bytes containing the contents of the file
   */
  static Csm::csmByte* load_file_as_bytes(const std::string filePath, Csm::csmSizeInt* outSize);

  /**
   * @brief Free a chunk of memory previously loaded from a file
   * 
   * @param[in] byteData
   */
  static void release_bytes(Csm::csmByte* byteData);

  /**
   * @brief Gets the time since the last frame was drawn
   */
  static Csm::csmFloat32 get_delta_time();

  /**
   * @brief Called after updating a frame
   */
  static void update_time();

  /**
   * @brief Log a message with printf formatting
   * 
   * @param[in] format The format string to print
   * @param[in] ... The variable arguments to the format string
   */
  static void print_log(const Csm::csmChar* format, ...);

  /**
   * @brief Log a message without formatting
   * 
   * @param[in] message The log message to print
   */
  static void print_message(const Csm::csmChar* message);

private:
  static double current_frame;
  static double last_frame;
  static double delta_time;
};

#endif /* LIVE2D_UTIL_HPP */