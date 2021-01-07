#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/objdetect.hpp>

namespace detector {
  class Detector {
  public:
    /* Accessor methods */
    const bool has_detected() const { return _has_detected; }
    const cv::Point& face_center() const { return _face_center; }
    const cv::Rect& face_roi() const { return _face_roi; }
    const cv::Point& eye_left() const { return _eye_left; }
    const cv::Point& eye_right() const { return _eye_right; }

    /* Methods to actually detect faces */
    void detect_face(cv::Mat frame);
    void draw_face(cv::Mat frame);

    /* Initialization method */
    bool load_classifiers(cv::String face_cascade_path, cv::String eyes_cascade_path);

    /* Class constructor */
    Detector();
  private:
    bool _has_detected;
    cv::Point _face_center;
    cv::Rect _face_roi;
    cv::Point _eye_left;
    cv::Point _eye_right;

    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;
  };
}

#endif /* DETECTOR_HPP */
