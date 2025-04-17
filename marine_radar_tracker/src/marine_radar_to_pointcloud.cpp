#pragma once
#include <rclcpp/rclcpp.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <cmath>

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
        "data", 100, std::bind(&MarineRadarToPointcloud::radarSectorCallback, this, _1));
  }

protected:
  void radarSectorCallback(const marine_sensor_msgs::msg::RadarSector::SharedPtr msg)
  {
    // First, count how many points we'll have to allocate memory efficiently
    size_t point_count = 0;
    for(size_t i = 0; i < msg->intensities.size(); i++) {
      for(size_t j = 0; j < msg->intensities[i].echoes.size(); j++) {
        if(msg->intensities[i].echoes[j] > detection_threshold_) {
          point_count++;
        }
      }
    }

    // Create PointCloud2 message
    auto cloud_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
    cloud_msg->header = msg->header;
    cloud_msg->height = 1;
    cloud_msg->width = point_count;

    // Add fields
    cloud_msg->fields.resize(4);
    cloud_msg->fields[0].name = "x";
    cloud_msg->fields[0].offset = 0;
    cloud_msg->fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg->fields[0].count = 1;

    cloud_msg->fields[1].name = "y";
    cloud_msg->fields[1].offset = 4;
    cloud_msg->fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg->fields[1].count = 1;

    cloud_msg->fields[2].name = "z";
    cloud_msg->fields[2].offset = 8;
    cloud_msg->fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg->fields[2].count = 1;

    cloud_msg->fields[3].name = "intensity";
    cloud_msg->fields[3].offset = 12;
    cloud_msg->fields[3].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg->fields[3].count = 1;

    // Set up point format
    cloud_msg->is_bigendian = false;
    cloud_msg->point_step = 16; // 4 fields * 4 bytes
    cloud_msg->row_step = cloud_msg->point_step * cloud_msg->width;
    cloud_msg->is_dense = true;

    // Allocate memory for the data
    cloud_msg->data.resize(cloud_msg->row_step);

    // Create point cloud iterators
    sensor_msgs::PointCloud2Iterator<float> iter_x(*cloud_msg, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(*cloud_msg, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(*cloud_msg, "z");
    sensor_msgs::PointCloud2Iterator<float> iter_intensity(*cloud_msg, "intensity");

    // Fill the point cloud
    for(size_t i = 0; i < msg->intensities.size(); i++) {
      double angle = msg->angle_start + i * msg->angle_increment;
      double c = cos(angle);
      double s = sin(angle);
      float range_increment = (msg->range_max - msg->range_min) / float(msg->intensities[i].echoes.size());

      for(size_t j = 0; j < msg->intensities[i].echoes.size(); j++) {
        if(msg->intensities[i].echoes[j] > detection_threshold_) {
          auto range = msg->range_min + j * range_increment;
          *iter_x = range * c;
          *iter_y = range * s;
          *iter_z = 0.0f;
          *iter_intensity = msg->intensities[i].echoes[j];

          // Move to next point
          ++iter_x;
          ++iter_y;
          ++iter_z;
          ++iter_intensity;
        }
      }
    }

    // Publish the point cloud
    pointcloud_publisher_->publish(*cloud_msg);
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
