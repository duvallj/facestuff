#include "ArgParse.hpp"

#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char** argv) {
  ArgumentConfig args(argc, argv);

  if (args.errored) {
    std::cerr << args.error_string << std::endl;
    return 1;
  }

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
    cv::imshow("Video", frame);
    if (cv::waitKey(24) >= 0) break;
  }

  std::cout << "Done!" << std::endl;

  return 0;
}
