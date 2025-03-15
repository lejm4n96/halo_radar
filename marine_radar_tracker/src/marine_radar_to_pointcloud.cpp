#pragma once
#include <rclcpp/rclcpp.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
//#include <pcl_ros/point_cloud.hpp> // not ported to ROS2

using std::placeholders::_1;
using namespace std;

class MarineRadarToPointcloud : public rclcpp::Node
{
public: 
  MarineRadarToPointcloud() : Node("marine_radar_to_pointcloud")
  {
    this->declare_parameter("detection_threshold", rclcpp::PARAMETER_DOUBLE);
    this->set_parameter(rclcpp::Parameter("detection_threshold", 0.0));

    //this->pointcloud_publisher_ = this->create_publisher<pcl::PointCloud<pcl::PointXYZI> >::SharedPtr("pointcloud", 10);
    //this->create_subscription<marine_sensor_msgs::msg::RadarSector>(
    //                                      "radar_data", 50, std::bind(&radarSectorCallback, this, _1));
  }

protected: 

  void radarSectorCallback(const marine_sensor_msgs::msg::RadarSector &msg) 
  {
    //ROS_INFO_STREAM("angle min: " << msg->angle_min << " angle max: " << msg->angle_max << " increment: " << msg->angle_increment);

    // hack for bug in earlier halo driver
    /*auto angle_increment = msg->angle_increment;
    if(angle_increment < 0.0)
      last_increment = angle_increment;
    else
      angle_increment = last_increment;


    if(!msg->intensities.empty())
    {
      pcl::PointCloud<pcl::PointXYZI> pc;
      pc.header.frame_id = msg->header.frame_id;
      pc.header.stamp = msg->header.stamp.toNSec()/1000;

      for(int i = 0; i < msg->intensities.size(); i++)
      {
        double angle = msg->angle_start + i*angle_increment;
        double c = cos(angle);
        double s = sin(angle);
        float range_increment = (msg->range_max - msg->range_min)/float(msg->intensities[i].echoes.size());

        for(int j = 0; j < msg->intensities[i].echoes.size(); j++)
        {
          if(msg->intensities[i].echoes[j] > detection_threshold)
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
      this->pointcloud_publisher_->publish(pc);
    }*/
  }

  //rclcpp::Subscription<marine_sensor_msgs::msg::RadarSector> radar_subscriber;
  rclcpp::Publisher<pcl::PointCloud<pcl::PointXYZI> >::SharedPtr pointcloud_publisher_;

  float detection_threshold = 0.0;
  float last_increment = 0.0;
};


int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  //ros::NodeHandle nh, pnh("~");
  //auto node = rclcpp::Node::make_shared("marine_radar_to_pointcloud");

  //detection_threshold = pnh.param("detection_threshold", 0.0);

  //ros::Subscriber radar_subscriber = nh.subscribe("radar_data", 50, &radarSectorCallback);
  //rclcpp::Subscription radar_subscriber = node->create_subscription<marine_sensor_msgs::msg::RadarSector>(
  //                                        "radar_data", 50, std::bind(&radarSectorCallback, _1));

  //pointcloud_publisher = pnh.advertise<pcl::PointCloud<pcl::PointXYZI> >("pointcloud", 10);

  rclcpp::spin(std::make_shared<MarineRadarToPointcloud>());
  return 0;
}    
