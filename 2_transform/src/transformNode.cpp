#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"
#include "tf/transform_listener.h"
#include <sstream>

void pubTransform(const sensor_msgs::LaserScan::ConstPtr& msg, ros::Publisher pub, tf::TransformListener* tListener){

  ros::Time startTime = ros::Time::now();
  std::string e;

  if (tListener->canTransform("odom","base_laser_link", startTime, &e)){
    tf::StampedTransform st;
    tListener->lookupTransform("odom", "base_laser_link", startTime, st);

    float x = st.getOrigin().x();
    float y = st.getOrigin().y();
    float theta = st.getRotation().getAngle();

    std::stringstream ss;
    ss << "Timestamp: " << st.stamp_;
    ss << " X: " << x;
    ss << " Y: " << y;
    ss << " Theta: " << theta;

    std_msgs::String newMsg;
    newMsg.data = ss.str();
    
    pub.publish(newMsg);
    ROS_INFO("Transform published on transformTopic");
  }
  else {
    ROS_INFO("Cannot transform");
  }

}

int main(int argc, char **argv){

  ros::init(argc, argv, "transformNode");

  tf::TransformListener tListener;

  ros::NodeHandle n;

  ros::Publisher pub;

  pub = n.advertise<std_msgs::String>("transformTopic", 1000);

  ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("base_scan", 1000, boost::bind(pubTransform, _1, pub, &tListener));

  ros::spin();

  return 0;
}
