#include "ArgParse.hpp"
extern "C" {
  #include "WinBGInput.h"
}

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

void detect_and_display(cv::Mat frame) {
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::imshow("Video", frame_gray);
}

int main(int argc, char** argv) {
  ArgumentConfig args(argc, argv);

  if (args.errored) {
    std::cerr << args.error_string << std::endl;
    return 1;
  }

  bg_input_init();

  cv::Mat frame;
  cv::VideoCapture cap;

  cap.open(args.camera_id);
  if (!cap.isOpened()) {
    std::cerr << "Error: cannot open camera \"" << args.camera_id << "\"" << std::endl;
    return 1;
  }

  std::cout << "Streaming from webcam..." << std::endl;

  while (1) {
    cap.read(frame);
    if (frame.empty()) {
      std::cout << "Blank frame encountered (hit end of video)" << std::endl;
      break;
    }
    detect_and_display(frame);
    if (cv::waitKey(24) >= 0) break;
  }

  std::cout << "Done!" << std::endl;

  bg_input_deinit();

  return 0;
}
