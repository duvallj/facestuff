#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>
#include <iostream>
#include <opencv2/core.hpp>

#include <string.h>

enum ARG_ENUM {
  ARG_CAMERA_ID,
  ARG_GAU_SIZE,
  ARG_GAU_STD,
  ARG_ENUM_SIZE,
};

static const std::string ARG_LIST[ARG_ENUM_SIZE] = {
  "-cam",
  "-gs",
  "-gd",
};

static const std::string usage_string = "\
Program usage: ./FaceStuff {OPTIONS}\n\
Available options:\n\
-cam [camera_id]         : (Default: 0) OpenCV id for camera to use\n\
-gs  [gaussian_size]     : (Default: 5) Size of gaussian kernel to\n\
                        apply when detecting features.\n\
-gd  [gaussian_std]      : (Default: 1.6) Standard deviation of gaussian\n\
                        kernel (with above)\n\
";

static const size_t ARG_OPT_BEGIN = 1;

class ArgumentConfig {
public:
  int camera_id          = 0;
  int gaussian_size      = 5;
  double gaussian_std    = 1.6;

  bool errored = false;
  std::string error_string;

  ArgumentConfig() {}
  ArgumentConfig(int argc, char** argv);
  ArgumentConfig(ArgumentConfig& settings);
};

#endif
