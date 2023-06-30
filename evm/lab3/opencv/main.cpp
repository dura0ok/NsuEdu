#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

std::vector<Point> points;

Mat draw(const Mat &captureFrame, const Mat &smallImage, int x, int y) {
  vector<Mat> rgba;
  split(smallImage, rgba);
  Mat rgb;
  merge(vector<Mat>{rgba[0], rgba[1], rgba[2]}, rgb);
  Mat alpha = rgba[3];

  Rect roi(Point(x, y), smallImage.size());
  Mat mask(smallImage.rows, smallImage.cols, CV_8UC1);

  Mat masked = captureFrame(roi);

  //copy alpha to masked
  rgb.copyTo(masked, alpha);
  captureFrame(roi) = masked;
  return captureFrame;
}

void onMouse(int event, int x, int y, int, void *data) {

  if (event == EVENT_LBUTTONDOWN) {
    points.emplace_back(x, y);
  }
}

int main(int, char **) {
  [[maybe_unused]] struct timespec startCode{}, endCode{};
  struct timespec loopStart{}, loopEnd{};
  clock_gettime(CLOCK_MONOTONIC_RAW, &startCode);
  double loopTime = 0;

  Mat captureFrame;
  VideoCapture camStream;
  int deviceID = 0;             // 0 = open default camera
  int apiID = cv::CAP_ANY;      // 0 = autodetect default API

  // open selected camera using selected API
  camStream.open(deviceID, apiID);
  cv::namedWindow("Live");


  // check if we succeeded
  if (!camStream.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }

  cv::Mat smallImage = imread("linux.png", cv::IMREAD_UNCHANGED);
  cv::setMouseCallback("Live", onMouse, nullptr);

  for (;;) {

    auto start = cv::getTickCount();
    clock_gettime(CLOCK_MONOTONIC_RAW, &loopStart);
    // wait for a new frame from camera and store it into 'frame'
    try {

      camStream.read(captureFrame);
      points.erase(unique(points.begin(), points.end()), points.end());
      for (auto &el : points) {
        captureFrame = draw(captureFrame, smallImage, el.x, el.y);
      }
    } catch (const std::exception &e) {
      std::cout << e.what();
      points.pop_back();
    }

    // check if we succeeded
    if (captureFrame.empty()) {
      cerr << "ERROR! blank frame grabbed\n";
      break;
    }
    // show live and wait for a key with timeout long enough to show images
    imshow("Live", captureFrame);

    /*
     * The cv.getTickCount function returns the number of clock-cycles after a reference event
     * (like the moment the machine was switched ON) to the moment this function is called.
     * So if you call it before and after the function execution, you get the number of clock-cycles used to execute a function.
     */

    /*
     * The cv.getTickFrequency function returns the frequency of clock-cycles,
     * or the number of clock-cycles per second. So to find the time of execution in seconds, you can do following
     */

    double fps = cv::getTickFrequency() / static_cast<double>((cv::getTickCount() - start));
    std::cout << "FPS : " << fps << std::endl;
    if (waitKey(30) == 27)
      break;
    clock_gettime(CLOCK_MONOTONIC_RAW, &loopEnd);
    loopTime += (double) loopEnd.tv_sec - (double) loopStart.tv_sec
        + 0.000000001 * ((double) loopEnd.tv_nsec - (double) loopStart.tv_nsec);
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &endCode);

  auto programFullTime = ((double) endCode.tv_sec - (double) startCode.tv_sec
      + 0.000000001 * ((double) endCode.tv_nsec - (double) startCode.tv_nsec));

  auto fraction = programFullTime / loopTime;
  cout << "program full time: " << programFullTime << endl;
  cout << "program loop time: " << loopTime << endl;
  cout << "fraction between program fool time and loop time " << fraction << endl;

  return 0;
}