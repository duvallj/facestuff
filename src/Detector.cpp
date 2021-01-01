#include "Detector.hpp"
#include <opencv2/imgproc.hpp>

using namespace detector;

Detector::Detector(void) {
  _has_detected = false;
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

/**
* @brief Given a frame, detect a face in it
* 
* Updates the internal state of the detector, which can be read out with accessor methods
* @param[in] frame The frame to detect a face in
* @pre The cascade classifiers should be initialized
*/
void Detector::detect_face(cv::Mat frame) {
  // Prepare frame for detection
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(frame_gray, frame_gray);
  
  // Run facial detection first
  std::vector<cv::Rect> faces;
  face_cascade.detectMultiScale(frame_gray, faces);

  // TODO: come up with better heuristic other than "first face seen"
  if (faces.size() >= 1) {
    _face_roi = faces[0];
    _face_center = cv::Point(_face_roi.x + _face_roi.width/2, _face_roi.y + _face_roi.height/2);

    // Then, within that face, detect the eyes
    cv::Mat face_roi_area = frame_gray(_face_roi);
    std::vector<cv::Rect> eyes;
    eyes_cascade.detectMultiScale(face_roi_area, eyes);

    if (eyes.size() >= 2) {
      // TODO: actuall distinguish between left and right
      _eye_left = cv::Point(_face_roi.x + eyes[0].x + eyes[0].width/2, _face_roi.y + eyes[0].y + eyes[0].height/2);
      _eye_right = cv::Point(_face_roi.x + eyes[1].x + eyes[1].width/2, _face_roi.y + eyes[1].y + eyes[1].height/2);
      _has_detected = true;
    } else {
      _has_detected = false;
    }
  } else {
    _has_detected = false;
  }
}

/**
 * @brief Given a frame that a face was detected in, draw the face on it
 *
 * This modifies the input frame
 * @param[in] frame The frame to draw on
 * @pre This should be the same frame that detect_face was called on, otherwise
 *      it doesn't make much sense
 */
void Detector::draw_face(cv::Mat frame) {
  if (_has_detected) {
    cv::ellipse(frame, _face_center, cv::Size(_face_roi.width / 2, _face_roi.height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 4);

    cv::circle(frame, _eye_left, 20, cv::Scalar(255, 0, 0), 4);
    cv::circle(frame, _eye_right, 20, cv::Scalar(0, 0, 255), 4);
  }
}
