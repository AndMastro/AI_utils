#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <homework5/moveAction.h>
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"

class moveAction {
protected:

ros::NodeHandle nh_;
actionlib::SimpleActionServer<homework5::moveAction> as_;   // NodeHandle instance must be created before this line. Otherwise strange error occurs.
std::string action_name_;
// create messages that are used to published feedback/result
homework5::moveResult result_;
ros::Publisher pub;
//pub = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
ros::Subscriber sub; // = nh_.subscribe<nav_msgs::Odometry>("odom", 1000, boost::bind(getOdom, _1, pub, goal));
bool first = true;
bool success = false;
nav_msgs::Odometry readOdom;


public:

moveAction(std::string name) :
        as_(nh_, name, boost::bind(&moveAction::executeCB, this, _1), false),
        action_name_(name) {
        pub = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
        sub = nh_.subscribe<nav_msgs::Odometry>("odom", 1000, &moveAction::getOdom, this);
        as_.start();
}

~moveAction(void)
{
}


void getOdom(const nav_msgs::Odometry::ConstPtr& msg){
        readOdom = *msg;
}



void executeCB(const homework5::moveGoalConstPtr &goal)
{

        success = true;

        if (as_.isPreemptRequested() || !ros::ok()) {
                ROS_INFO("%s: Preempted", action_name_.c_str());
                as_.setPreempted();
                success = false;

        }

        else {


                while (first) {
                        ros::spinOnce();
                        first = false;
                }

                nav_msgs::Odometry startOdom = readOdom;
                geometry_msgs::Twist move;
                move.linear.x = goal->desired_speed;



                while (sqrt(pow(startOdom.pose.pose.position.x - readOdom.pose.pose.position.x, 2) + pow(startOdom.pose.pose.position.y - readOdom.pose.pose.position.y, 2)) <  goal->distance) {
                        ROS_INFO("Moving the robot");
                        pub.publish(move);
                        ros::spinOnce;
                }

                result_.odom_pose = readOdom;


        }

        if(success)
        {
                ROS_INFO("%s: Succeeded", action_name_.c_str());

                as_.setSucceeded(result_);
        } else {

                ROS_INFO("%s: FAILED", action_name_.c_str());

        }



        success = false;


}


};


int main(int argc, char** argv)
{
        ros::init(argc, argv, "move");

        moveAction move("move");
        ros::spin();

        return 0;
}
