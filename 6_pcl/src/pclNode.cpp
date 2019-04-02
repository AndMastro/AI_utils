#include "ros/ros.h"
#include "sensor_msgs/PointCloud2.h"
#include "sensor_msgs/LaserScan.h"

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <iostream>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/passthrough.h>

#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/visualization/cloud_viewer.h>

using namespace std;
using namespace pcl;

pcl::visualization::CloudViewer viewer("Cloud Viewer");
pcl::PassThrough<PointXYZ> pass_through;
pcl::VoxelGrid<PointXYZ> voxel_grid;

void pclCallback(const sensor_msgs::PointCloud2::ConstPtr& input){

	//converting
	pcl::PointCloud<pcl::PointXYZ>:: Ptr cloud_ptr(new PointCloud<PointXYZ>());
	pcl::fromROSMsg(*input, *cloud_ptr);


 //pass_through fliter

	pass_through.setInputCloud (cloud_ptr);
	pass_through.setFilterLimits (0.5, 10);
	pass_through.setFilterFieldName ("z");
	pass_through.filter( *cloud_ptr );

	//apply voxelization

	voxel_grid.setInputCloud (cloud_ptr);
	voxel_grid.setLeafSize (0.1, 0.1, 0.1);
	voxel_grid.filter ( *cloud_ptr );



	//visualize
	viewer.showCloud(cloud_ptr);



}

int main(int argc, char **argv){

	ros::init(argc, argv, "pclNode");

	ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe<sensor_msgs::PointCloud2>("camera/depth/points", 1000, pclCallback);

	ros::spin();

  return 0;
}
