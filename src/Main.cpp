#include "Detector.hpp"
extern "C" {
  #include "WinBGInput.h"
}

#include <opencv2/highgui.hpp>
#include <iostream>

static void help(const char **argv) {
std::cout << "Program usage: " << argv[0] << " {OPTIONS}\n"
"Available options:\n"
"--cam=<camera_id>         : (Default: 0) OpenCV id for camera to use\n"
"--gs=<gaussian_size>      : (Default: 5) Size of gaussian kernel to\n"
"                            apply when detecting features.\n"
"--gd=<gaussian_std>       : (Default: 1.6) Standard deviation of gaussian\n"
"                            kernel (with above)\n"
"--face-cascade=<path>     : (Default: \"data/haarcascades/haarcascade_frontalface_alt.xml\")\n"
"                            Controls which Haar cascade config file to use for\n"
"                            face detection\n"
"--eyes-cascade=<path>     : (Default: \"data/haarcascades/haarcascade_eye_tree_eyeglasses.xml\")\n"
"                            Controls which Haar cascade config file to use for\n"
"                            eye detection within the face\n" << std::endl;
}

void detect_and_display(cv::Mat frame, detector::Detector dct) {
  dct.detect_face(frame);
  if (dct.has_detected()) {
    dct.draw_face(frame);
  }
  cv::imshow("Video", frame);
}

int main(int argc, const char** argv) {
  cv::CommandLineParser parser(argc, argv,
      "{help h||}"
      "{cam|0|}"
      "{gs|5|}"
      "{gd|1.6|}"
      "{face-cascade|data/haarcascades/haarcascade_frontalface_alt.xml|}"
      "{eyes-cascade|data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|}"
  );

  if (parser.has("help")) {
    help(argv);
    return 0;
  }

  cv::String face_cascade_name = cv::samples::findFileOrKeep(parser.get<cv::String>("face-cascade"));
  cv::String eyes_cascade_name = cv::samples::findFileOrKeep(parser.get<cv::String>("eyes-cascade"));

  cv::Mat frame;
  cv::VideoCapture cap;
  detector::Detector dct;

  if (!dct.load_classifiers(face_cascade_name, eyes_cascade_name)) {
    std::cerr << "Error: cannot open face and eyes cascade files \"" << face_cascade_name << "\" and \"" << eyes_cascade_name << "\"." << std::endl;
    return 1;
  }

  int camera_id = parser.get<int>("cam");
  cap.open(camera_id);
  if (!cap.isOpened()) {
    std::cerr << "Error: cannot open camera \"" << camera_id << "\"" << std::endl;
    return 1;
  }

  bg_input_init();
  std::cout << "Streaming from webcam..." << std::endl;

  while (1) {
    cap.read(frame);
    if (frame.empty()) {
      std::cout << "Blank frame encountered (hit end of video)" << std::endl;
      break;
    }
    detect_and_display(frame, dct);
    if (cv::waitKey(24) >= 0) break;
  }

  std::cout << "Done!" << std::endl;

  bg_input_deinit();

  return 0;
}
