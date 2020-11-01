#include "ArgParse.hpp"


ArgumentConfig::ArgumentConfig(ArgumentConfig& settings) {
  this->operator=(settings);
}

ArgumentConfig::ArgumentConfig(int argc, char** argv) {
  if (argc < ARG_OPT_BEGIN) {
    error_string = "";
    errored = true;
    goto arg_error;
  } else {
    std::string arg_key;
    std::string arg_val;

    for (int i=ARG_OPT_BEGIN; i<argc; i++) {
      arg_key = std::string(argv[i]);
      if (arg_key[0] != '-') {
        error_string = "Error: argument name \"" + arg_key + "\" is malformed\n";
        errored = true;
        goto arg_error;
      } else {
        if (i+1>=argc) {
          error_string = "Error: did not find argument after \"" + arg_key + "\"\n";
          errored = true;
          goto arg_error;
        } else {
          arg_val = std::string(argv[i+1]);
          i++;

          int arg_key_index = -1;
          for (int c=0; c<ARG_ENUM_SIZE; c++) {
            if (arg_key == ARG_LIST[c]) {
              arg_key_index = c;
              break;
            }
          }

          switch (arg_key_index) {
            case ARG_CAMERA_ID:
              camera_id = std::stoi(arg_val);
              break;
            case ARG_GAU_SIZE:
              gaussian_size = std::stoi(arg_val);
              break;
            case ARG_GAU_STD:
              gaussian_std = std::stod(arg_val);
              break;
            default:
              error_string = "Error: argument name \"" + arg_key + "\" is invalid\n";
              errored = true;
              goto arg_error;
              break;
          }
        }
      }
    }
  }
  if (errored) {
arg_error:
    error_string += usage_string;
  }
}
