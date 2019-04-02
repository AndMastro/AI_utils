#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"
#include <limits.h>
#include <sstream>

void printMinDistance(const sensor_msgs::LaserScan::ConstPtr& msg, ros::Publisher pub){

  int dim = (msg->ranges).size();
  float min = FLT_MAX;
  for (int i = 0; i < dim; ++i){
    if ((msg->ranges)[i] < min) min = (msg->ranges)[i];
  }

  std_msgs::String newMsg;

  std::stringstream ss;

  ss << min;
  newMsg.data = ss.str();

  pub.publish(newMsg);

}

int main(int argc, char **argv){

  ros::init(argc, argv, "laserMin");

  ros::NodeHandle n;

  ros::Publisher pub;

  pub = n.advertise<std_msgs::String>("minDist", 1000);

  ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("base_scan", 1000, boost::bind(printMinDistance, _1, pub));

  ros::spin();

  return 0;
}
