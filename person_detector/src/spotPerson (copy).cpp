#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include "sensor_msgs/Image.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "geometry_msgs/PoseArray.h"
#include "std_msgs/String.h"
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <limits.h>
#include <sstream>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* ac;
bool spotted = false;
int num = 0;

void checkIntrusion(const geometry_msgs::PoseArray::ConstPtr& msg){
  geometry_msgs::PoseArray spot = *msg;
  if (spotted == false){
  if (spot.poses.size() > 0 && !std::isnan(spot.poses[0].position.x) && spot.poses[0].position.x <= 3.0){

	spotted = true;

    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");




    //wait for the action server to come up
    while(!ac->waitForServer(ros::Duration(5.0))) {
            ROS_INFO("Waiting for the move_base action server to come up");
    }

    /*move_base_msgs::MoveBaseGoal stopGoal;
    stopGoal.target_pose.header.frame_id = "base_link";
    stopGoal.target_pose.header.stamp = ros::Time::now();
    stopGoal.target_pose.pose.position.x = 0;
    stopGoal.target_pose.pose.position.y = 0;
    stopGoal.target_pose.pose.orientation.w = 0;*/

    //(*ac).cancelAllGoals();

    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");
    ROS_INFO("INTRUSION DETECTED");

    //(ac->sendGoal)(stopGoal);
    //ac->waitForResult();

    //AGGIUNGERE CANCEL GOALS

  }
}



}

void takeSnapshot(const sensor_msgs::ImageConstPtr& msg){

	if (spotted == true){
		cv_bridge::CvImageConstPtr cv_ptr;

		try {
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
		}
		catch (cv_bridge::Exception& ex){

			ROS_ERROR("cv_bridge exception: %s", ex.what());
			exit(EXIT_FAILURE);
		}
		std::stringstream ss;
		ss<<"/home/mastro/shots/snapshot_"<<num<<".jpg";
		cv::imwrite(ss.str(),cv_ptr->image);
		num++;
		ROS_INFO("A snaphsot has been saved.");
		spotted = false;
	//cv::waitKey(30); //!!!!!!
    //ros::shutdown();
	}
}

int main(int argc, char **argv){

  ros::init(argc, argv, "spotPerson");

  ros::NodeHandle n;

  //ros::Publisher pub;

  //pub = n.advertise<std_msgs::String>("minDist", 1000);

  ac = new MoveBaseClient("move_base", true);

  ros::Subscriber sub = n.subscribe<geometry_msgs::PoseArray>("edge_leg_detector", 1000, checkIntrusion);

  ros::Subscriber sub1 = n.subscribe<sensor_msgs::Image>("camera/rgb/image_color", 1000, takeSnapshot);

  ros::spin();

  return 0;
}
