#include "Detector.hpp"
#include <opencv2/imgproc.hpp>

using namespace detector;

Detector::Detector(void) {
  // Pass
}

bool Detector::load_classifiers(cv::String face_cascade_path, cv::String eyes_cascade_path) {
  if (!face_cascade.load(face_cascade_path)) {
    return false;
  }
  if (!eyes_cascade.load(eyes_cascade_path)) {
    return false;
  }

  return true;
}

void Detector::detect_face(cv::Mat frame) {
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(frame_gray, frame_gray);
  std::vector<cv::Rect> faces;
  face_cascade.detectMultiScale(frame_gray, faces);
}
