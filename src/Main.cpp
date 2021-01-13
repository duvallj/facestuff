#include "Detector.hpp"
extern "C" {
  #include "WinBGInput.h"
}

#include "live2d/Displayer.hpp"
#include "live2d/Util.hpp"

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

class MainState {
public:
  Displayer* disp;
  cv::VideoCapture cap;
  detector::Detector dct;

  static bool tick(void* obj) {
    MainState* state = reinterpret_cast<MainState*>(obj);
    return state->tick();
  }

  bool tick() {
    cap.read(_frame);
    if (_frame.empty()) {
      std::cout << "Blank frame encountered (hit end of video)" << std::endl;
      return false;
    }
    detect_and_display(_frame, dct);
    return true;
  }

private:
  cv::Mat _frame;
};

void mainloop(MainState* state) {
  ACGL_thread_t* main_thread = ACGL_thread_create(
    NULL, // No setup required
    MainState::tick,
    NULL, // No cleanup required
    16, // 16ms roughly equals 60fps
    state,
    NULL
  );

  if (main_thread == NULL) {
    fprintf(stderr, "Error, could not start main_thread: %s\n", SDL_GetError());
    return;
  }
  if (ACGL_thread_start(main_thread, "main_thread") != 0) {
    fprintf(stderr, "Error while starting main_thread: %s\n", SDL_GetError());
    return;
  }

  SDL_Event e;
  if (!state->disp->get_is_end() && SDL_WaitEvent(&e) != 0) {
    switch (e.type) {
    case SDL_QUIT:
      state->disp->app_end();
      break;
    case SDL_WINDOWEVENT:
      // Handle with ACGL
      break;
    case SDL_KEYUP:
    case SDL_KEYDOWN:
      // Handle with ACGL
      break;
    case SDL_USEREVENT:
      // Custom handlers, more to be added later
      switch (e.user.code) {
      case Displayer::RefreshRequest:
        LAppUtil::update_time();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        state->disp->render();
        break;
      default:
        break;
      }
      break;
    }
  }

  if (ACGL_thread_stop(main_thread) != 0) {
    fprintf(stderr, "Error stopping main_thread: %s\n", SDL_GetError());
    return;
  }
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

  MainState* state = new MainState();

  if (!state->dct.load_classifiers(face_cascade_name, eyes_cascade_name)) {
    std::cerr << "Error: cannot open face and eyes cascade files \"" \
      << face_cascade_name << "\" and \"" \
      << eyes_cascade_name << "\"." \
      << std::endl;
    return 1;
  }

  int camera_id = parser.get<int>("cam");
  state->cap.open(camera_id);
  if (!state->cap.isOpened()) {
    std::cerr << "Error: cannot open camera \"" \
      << camera_id << "\"" \
      << std::endl;
    return 1;
  }
  std::cout << "Camera Opened." << std::endl;

  bg_input_init();
  std::cout << "Background keyboard hook initialized." << std::endl;

  state->disp = new Displayer();
  std::cout << "Display opened with OpenGL." << std::endl;

  mainloop(state);

  std::cout << "Done!" << std::endl;

  delete state->disp;
  bg_input_deinit();

  return 0;
}
