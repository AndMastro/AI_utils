#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"
#include <limits.h>
#include <sstream>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#define SCALE 30
#define UP 200

void printLaserMap(const sensor_msgs::LaserScan::ConstPtr& msg){

  cv::Mat img(400, 480, CV_8UC3, cv::Scalar(255,255,255));
  int dim = (msg->ranges).size();
  int iMin;
  float min = FLT_MAX;
  for (int i = 0; i < dim; ++i){
    if ((msg->ranges)[i] < min) {
      min = (msg->ranges)[i];
      iMin = i;
    }
  }

  float xm = min*SCALE*cos((msg->angle_max) - ((msg->angle_increment)*iMin)) + UP;
  float ym = min*SCALE*sin((msg->angle_max) - ((msg->angle_increment)*iMin)) + UP;
  cv::Point2f* pm = new cv::Point2f(xm,ym);
  cv::circle(img, *pm, 5, cv::Scalar(255,0,0), 1, cv::LINE_8, 0);

  for(int i = 0; i + 1 < dim; ++i){
    float x1 = ((msg->ranges)[i]*SCALE*cos((msg->angle_max) - ((msg->angle_increment)*i))) + UP;
    float y1 = ((msg->ranges)[i]*SCALE*sin((msg->angle_max) - ((msg->angle_increment)*i)))+ UP;
    printf("%f  %f\n", x1, y1);
    cv::Point2f* p1 = new cv::Point2f(x1,y1);
    float x2 = ((msg->ranges)[i + 1]*SCALE*cos((msg->angle_max) - ((msg->angle_increment)*(i+1))))+ UP;
    float y2 = ((msg->ranges)[i  +1]*SCALE*sin((msg->angle_max) - ((msg->angle_increment)*(i+1))))+ UP;
    cv::Point2f* p2 = new cv::Point2f(x2,y2);
    cv::line(img, *p1, *p2, cv::Scalar(0,0,255), 1, cv::LINE_8, 0);
  }

  cv::imshow("LaserMap", img);
  cv::waitKey(30);

}

int main(int argc, char **argv){

  ros::init(argc, argv, "laserDraw");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("base_scan", 1000, printLaserMap);

  ros::spin();

  return 0;
}
