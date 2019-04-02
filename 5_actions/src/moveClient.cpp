#include <ros/ros.h>
#include <homework5/moveAction.h>
#include <actionlib/client/terminal_state.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<homework5::moveAction> moveClient;

int main(int argc, char** argv){
        ros::init(argc, argv, "moveClient");

        //tell the action client that we want to spin a thread by default
        moveClient ac("move", true);

        //wait for the action server to come up
        while(!ac.waitForServer(ros::Duration(5.0))) {
                ROS_INFO("Waiting for the move action server to come up");
        }

        homework5::moveGoal goal;

        //I'll send a goal to the robot to move 3.5 meter forward
        goal.desired_speed = 25;
        goal.distance = 3.5;

        ROS_INFO("Sending goal");
        ac.sendGoal(goal);

        bool finished_before_timeout = ac.waitForResult(ros::Duration(30.0));

        if (finished_before_timeout)
        {
                actionlib::SimpleClientGoalState state = ac.getState();
                ROS_INFO("Action finished: %s",state.toString().c_str());
              }
        else
                ROS_INFO("Action did not finish before the time out.");

        return 0;
}
