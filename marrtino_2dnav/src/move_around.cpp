#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>

int place = 4; //lab

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
        ros::init(argc, argv, "move_around");

        sleep(5);

        MoveBaseClient ac("move_base", true);

        while(!ac.waitForServer(ros::Duration(5.0))) {
                ROS_INFO("Waiting for the move_base action server to come up");
        }

        move_base_msgs::MoveBaseGoal goals[place];

        tf::Quaternion quaternion;
        geometry_msgs::Quaternion qMsg;
        double radians;
        double theta;

        if (place == 6) {
                goals[0].target_pose.header.frame_id = "map";
                goals[0].target_pose.header.stamp = ros::Time::now();
                goals[0].target_pose.pose.position.x = 1.41996669769;
                goals[0].target_pose.pose.position.y = 0.696592330933;
                goals[0].target_pose.pose.orientation.w = 1.0;

                goals[1].target_pose.header.frame_id = "map";
                goals[1].target_pose.header.stamp = ros::Time::now();
                goals[1].target_pose.pose.position.x = 2.38517570496;
                goals[1].target_pose.pose.position.y = -0.350340843201;
                goals[1].target_pose.pose.orientation.w = 1.0;

                goals[2].target_pose.header.frame_id = "map";
                goals[2].target_pose.header.stamp = ros::Time::now();
                goals[2].target_pose.pose.position.x = 1.41996669769;
                goals[2].target_pose.pose.position.y = 0.696592330933;
                goals[2].target_pose.pose.orientation.w = 1.0;

                goals[3].target_pose.header.frame_id = "map";
                goals[3].target_pose.header.stamp = ros::Time::now();
                goals[3].target_pose.pose.position.x = 2.2764377594;
                goals[3].target_pose.pose.position.y = 2.81526851654;
                goals[3].target_pose.pose.orientation.w = 1.0;

                goals[4].target_pose.header.frame_id = "map";
                goals[4].target_pose.header.stamp = ros::Time::now();
                goals[4].target_pose.pose.position.x = 1.41996669769;
                goals[4].target_pose.pose.position.y = 0.696592330933;
                goals[4].target_pose.pose.orientation.w = 1.0;

                goals[5].target_pose.header.frame_id = "map";
                goals[5].target_pose.header.stamp = ros::Time::now();
                goals[5].target_pose.pose.position.x = -1.21812009811;
                goals[5].target_pose.pose.position.y = 2.19211149216;
                goals[5].target_pose.pose.orientation.w = 1.0;
        }

        if (place == 4) {
                goals[0].target_pose.header.frame_id = "map";
                goals[0].target_pose.header.stamp = ros::Time::now();
                goals[0].target_pose.pose.position.x = -0.865;
                goals[0].target_pose.pose.position.y = -1.03;
                theta = -120;
                radians = theta * (M_PI/180);
                quaternion = tf::createQuaternionFromYaw(radians);
                tf::quaternionTFToMsg(quaternion, qMsg);
                goals[0].target_pose.pose.orientation = qMsg;

                goals[1].target_pose.header.frame_id = "map";
                goals[1].target_pose.header.stamp = ros::Time::now();
                goals[1].target_pose.pose.position.x = 7.44;
                goals[1].target_pose.pose.position.y = 0.0427;
                theta = -110;
                radians = theta * (M_PI/180);
                quaternion = tf::createQuaternionFromYaw(radians);
                tf::quaternionTFToMsg(quaternion, qMsg);
                goals[1].target_pose.pose.orientation = qMsg;

                goals[2].target_pose.header.frame_id = "map";
                goals[2].target_pose.header.stamp = ros::Time::now();
                goals[2].target_pose.pose.position.x = 13.2;
                goals[2].target_pose.pose.position.y = 0.7;
                theta = 0;
                radians = theta * (M_PI/180);
                quaternion = tf::createQuaternionFromYaw(radians);
                tf::quaternionTFToMsg(quaternion, qMsg);
                goals[2].target_pose.pose.orientation = qMsg;
                
                goals[3].target_pose.header.frame_id = "map";
                goals[3].target_pose.header.stamp = ros::Time::now();
                goals[3].target_pose.pose.position.x = 13.1;
                goals[3].target_pose.pose.position.y = 2.3;
                theta = 90;
                radians = theta * (M_PI/180);
                quaternion = tf::createQuaternionFromYaw(radians);
                tf::quaternionTFToMsg(quaternion, qMsg);
                goals[3].target_pose.pose.orientation = qMsg;
        }

        while(1) {
                for (int i = 0; i < place; ++i) {

                        ROS_INFO("Sending goal");
                        ac.sendGoal(goals[i]);
                        ac.waitForResult();

                        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                                ROS_INFO("Hooray, the robot moved successfully!");
                        else    {
                                ROS_INFO("The robot stopped");
                                return 0;
                        }
                }
        }
        return 0;
}
