#pragma once
#include <rclcpp/rclcpp.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/msg/point_cloud2.hpp>
//#include <pcl_ros/point_cloud.hpp> // not ported to ROS2

using std::placeholders::_1;
using namespace std;

class MarineRadarToPointcloud : public rclcpp::Node
{
public: 
  MarineRadarToPointcloud() : Node("marine_radar_to_pointcloud")
  {
    this->declare_parameter("detection_threshold", rclcpp::PARAMETER_DOUBLE);
    this->set_parameter(rclcpp::Parameter("detection_threshold", this->detection_threshold_));

    this->pointcloud_publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("pointcloud", 10);
    this->radar_subscriber_ = this->create_subscription<marine_sensor_msgs::msg::RadarSector>(
                             "data", 50, std::bind(&MarineRadarToPointcloud::radarSectorCallback, this, _1));
  }

protected: 

  void radarSectorCallback(const marine_sensor_msgs::msg::RadarSector::SharedPtr msg) 
  {
    //ROS_INFO_STREAM("angle min: " << msg->angle_min << " angle max: " << msg->angle_max << " increment: " << msg->angle_increment);

    // hack for bug in earlier halo driver
    auto angle_increment = msg->angle_increment;
    if(angle_increment < 0.0)
      last_increment_ = angle_increment;
    else
      angle_increment = last_increment_;

    if(!msg->intensities.empty())
    {
      pcl::PointCloud<pcl::PointXYZI> pc;
      //pc.header.frame_id = msg->header.frame_id;        // Doing these when converting to PointCloud2 msg 
      //pc.header.stamp = msg->header.stamp.toNSec()/1000; 

      for(int i = 0; i < msg->intensities.size(); i++)
      {
        double angle = msg->angle_start + i*angle_increment;
        double c = cos(angle);
        double s = sin(angle);
        float range_increment = (msg->range_max - msg->range_min)/float(msg->intensities[i].echoes.size());

        for(int j = 0; j < msg->intensities[i].echoes.size(); j++)
        {
          if(msg->intensities[i].echoes[j] > detection_threshold_)
          {
            auto range = msg->range_min+ j*range_increment;
            pcl::PointXYZI p;
            p.x = range*c;
            p.y = range*s;
            p.z = 0.0;
            p.intensity = msg->intensities[i].echoes[j];
            pc.push_back(p);
          }
        }
      }

      // Convert PCL-type point cloud to ROS sensor_msgs PointCloud2 type
      // This is because <pcl_ros/point_cloud.hpp> is not fully ported to ROS2 
      // and ROS2 doesn't seem to accept the PCL-type point cloud as a publisher template 
      // Maybe can eliminate this conversion step later on if pcl_ros is fully ported?
      auto pc2_ros_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
      pcl::toROSMsg(pc, *pc2_ros_msg);
      pc2_ros_msg->header.frame_id = msg->header.frame_id;
      pc2_ros_msg->header.stamp = msg->header.stamp;
      this->pointcloud_publisher_->publish(*pc2_ros_msg);
    }
  }

  rclcpp::Subscription<marine_sensor_msgs::msg::RadarSector>::SharedPtr radar_subscriber_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pointcloud_publisher_;

  float detection_threshold_ = 0.0;
  float last_increment_ = 0.0;
};


int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MarineRadarToPointcloud>());
  rclcpp::shutdown();
  return 0;
}    
