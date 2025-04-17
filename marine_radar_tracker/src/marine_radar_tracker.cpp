/* APRIL 17, 2025: This node has NOT BEEN PORTED to ROS2 yet. Code is retained here in a partially-ported state
but the tracker is being rewritten so this code is on hold for now. */

/*#pragma once
#include <rclcpp/rclcpp.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <project11_msgs/msg/detect.hpp>
#include <grid_map_ros/grid_map_ros.hpp>
#include <tf2/utils.h>
#include <future>
#include "target.h"

using std::placeholders::_1;


namespace marine_radar_tracker
{

class MarineRadarTracker : public rclcpp::Node
{
public:
  MarineRadarTracker() 
  : Node("marine_radar_tracker"), 
    tf_listener_(tf_buffer_), 
    grid_map_({"intensity", "latest", "latest_age", "previous", "previous_age"})
  {

    tf_buffer_(this->get_clock());
    this->declare_parameter("minimum_range", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("detection_threshold", rclcpp::PARAMETER_DOUBLE);  
    this->declare_parameter("map_frame", rclcpp::PARAMETER_STRING);
    this->declare_parameter("grid_resolution_factor", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("grid_length_factor", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("sensor_id", rclcpp::PARAMETER_STRING);
    this->declare_parameter("publish_interval", rclcpp::Duration);

    this->set_parameter(rclcpp::Parameter("minimum_range", this->minimum_range_;))
    this->set_parameter(rclcpp::Parameter("detection_threshold", this->detection_threshold_));
    this->set_parameter(rclcpp::Parameter("map_frame", this->map_frame_));
    this->set_parameter(rclcpp::Parameter("grid_resolution_factor", this->grid_resolution_factor_));
    this->set_parameter(rclcpp::Parameter("grid_length_factor", this->grid_length_factor_));
    this->set_parameter(rclcpp::Parameter("sensor_id", this->sensor_id_));
    this->set_parameter(rclcpp::Parameter("publish_interval", this->publish_interval_));

    grid_map_.setFrameId(map_frame_);
    double interval = publish_interval_.seconds();
    interval = pnh.param("publish_interval", interval);
    publish_interval_.from_seconds(interval);

    //ros::NodeHandle nh, pnh("~");

    //minimum_range_ = pnh.param("minimum_range", minimum_range_);
    //detection_threshold_ = pnh.param("detection_threshold", detection_threshold_);
    //map_frame_ = pnh.param("map_frame", map_frame_);

    //grid_resolution_factor_ = pnh.param("grid_resolution_factor", grid_resolution_factor_);
    //grid_length_factor_ = pnh.param("grid_length_factor", grid_length_factor_);

    //sensor_id_ = pnh.param("sensor_id", sensor_id_);

    //radar_subscriber_ = nh.subscribe("radar_data", 100, &MarineRadarTracker::radarSectorCallback, this);
    this->radar_subscriber_ = this->create_subscription<marine_sensor_msgs::msg::RadarSector>("radar_data", 100, 
                        std::bind(&MarineRadarTracker::radarSectorCallback, this, _1));
    
    //markers_publisher_ = nh.advertise<visualization_msgs::MarkerArray>("radar_markers", 10);
    this->markers_publisher_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("radar_markers", 10);
    //grid_map_publisher_ = nh.advertise<grid_map_msgs::GridMap>("grid_map", 1, true);
    this->grid_map_publisher_ = this->create_publisher<grid_map_msgs::msg::GridMap>("grid_map", 1, true);
    //detects_publisher_ = nh.advertise<project11_msgs::msg::Detect>("detects", 250);
    this->detects_publisher_ = this->create_publisher<project11_msgs::msg::Detect>("detects", 250);
  }

private:
  rclcpp::Subscription<marine_sensor_msgs::msg::RadarSector>::SharedPtr radar_subscriber_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr markers_publisher_;
  rclcpp::Publisher<grid_map_msgs::msg::GridMap>::SharedPtr grid_map_publisher_;
  rclcpp::Publisher<project11_msgs::msg::Detect>::SharedPtr detects_publisher_;

  float detection_threshold_ = 0.0;
  float minimum_range_ = 0.0;

  std::string map_frame_ = "map";
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  grid_map::GridMap grid_map_;
  float grid_resolution_factor_ = 6.5;
  float grid_length_factor_ = 0.6;

  rclcpp::Time last_time_;
  double last_range_ = 0.0;

  rclcpp::Time last_target_scan_time_;
  std::future<bool> scan_done_ = std::future<bool>();

  rclcpp::Time last_publish_time_;
  rclcpp::Duration publish_interval_ = rclcpp::Duration::from_seconds(0.2);

  std::string sensor_id_ = "radar";


  void radarSectorCallback(const marine_sensor_msgs::msg::RadarSector::SharedPtr msg)
  {

    // TODO: I have no idea what to do with simTime in ros2 and can't find a single
    // scrap of documentation mentioning it. 
    //if(ros::Time::isSimTime() && msg->header.stamp < last_time_)
    if(rclcpp::Time(msg->header.stamp) < last_time_)
    {
      grid_map_.clearAll();
      last_target_scan_time_ = rclcpp::Time();
      last_publish_time_ = rclcpp::Time();
    }

    if(msg->range_max != last_range_)
    {
      float grid_size = msg->range_max * 2.0 * grid_length_factor_;
      float resolution = grid_resolution_factor_* (msg->range_max - msg->range_min) / 
                                                   float(msg->intensities.front().echoes.size());
      grid_map_.setGeometry(grid_map::Length(grid_size, grid_size), resolution);
      last_range_ = msg->range_max;
    }

    geometry_msgs::msg::TransformStamped transform;
    try{
      transform = tf_buffer_.lookupTransform(map_frame_, msg->header.frame_id, msg->header.stamp, rclcpp::Duration::from_seconds(0.1));
    }
    catch (tf2::TransformException &ex) {
      RCLCPP_WARN_STREAM(this->get_logger(), ex.what());
      return;
    }

    geometry_msgs::msg::PoseStamped p;
    p.header = msg->header;
    p.pose.orientation.w = 1.0;

    geometry_msgs::msg::PoseStamped radar_in_map_frame;
    tf2::doTransform(p, radar_in_map_frame, transform);

    auto yaw = tf2::getYaw(radar_in_map_frame.pose.orientation);

    grid_map::Position grid_center(radar_in_map_frame.pose.position.x, radar_in_map_frame.pose.position.y);
    grid_map_.move(grid_center);

    double dt = std::max(0.0, (rclcpp::Time(msg->header.stamp) - last_time_).seconds());

    for(grid_map::GridMapIterator it(grid_map_); !it.isPastEnd(); ++it)
    {
      if(!isnan(grid_map_.at("latest_age",*it)))
      {
        double new_age = grid_map_.at("latest_age",*it) + dt;
        if(new_age > 5.0)
        {
          grid_map_.at("latest_age",*it) = std::nan("");
          grid_map_.at("latest",*it) = std::nan("");
        }
        else
          grid_map_.at("latest_age",*it) = new_age;
      }
      if(!isnan(grid_map_.at("previous_age",*it)))
      {
        double new_age = grid_map_.at("previous_age",*it) + dt;
        if(new_age > 5.0)
        {
          grid_map_.at("previous_age",*it) = std::nan("");
          grid_map_.at("previous",*it) = std::nan("");
        }
        else
          grid_map_.at("previous_age",*it) = new_age;
      }

      grid_map::Position position;
      grid_map_.getPosition(*it, position);
      double dx = position.x() - radar_in_map_frame.pose.position.x;
      double dy = position.y() - radar_in_map_frame.pose.position.y;
      double r = sqrt(dx*dx+dy*dy);
      if(r >= minimum_range_ && r <= msg->range_max && r >= msg->range_min)
      {
        double theta = atan2(dy, dx);
        if (theta < 0.0)
          theta += 2.0*M_PI;
        theta -= yaw;
        if(theta < 0.0)
          theta += 2.0*M_PI;
        if (theta > 2.0*M_PI)
          theta -= 2.0*M_PI;
        int i = (theta-(msg->angle_start-msg->angle_increment/2.0))/msg->angle_increment;
        if(i >= 0 && i < msg->intensities.size())
        {
          int j = (msg->intensities[i].echoes.size()-1)*(r-msg->range_min)/(msg->range_max-msg->range_min);
          float intensity = msg->intensities[i].echoes[j];
          if(!isnan(grid_map_.at("latest_age", *it)) && grid_map_.at("latest_age", *it) > 0.1)
          {
            grid_map_.at("previous", *it) = grid_map_.at("latest", *it);
            grid_map_.at("previous_age", *it) = grid_map_.at("latest_age", *it);
          }
          grid_map_.at("latest", *it) = intensity;
          grid_map_.at("latest_age", *it) = 0.0;
        }
      }
      if(isnan(grid_map_.at("latest", *it)))
        grid_map_.at("intensity", *it) = std::nan("");
      else
        if(isnan(grid_map_.at("previous", *it)))
          grid_map_.at("intensity", *it) = grid_map_.at("latest", *it)*.5;
        else
          grid_map_.at("intensity", *it) = 0.5*(grid_map_.at("latest", *it)+ grid_map_.at("previous", *it)); //std::min(grid_map_.at("latest", *it), grid_map_.at("previous", *it));
    }

    grid_map_.setTimestamp(msg->header.stamp.toNSec());
    if(rclcpp::Time(msg->header.stamp) >= last_publish_time_+publish_interval_)
    {
      grid_map_msgs::msg::GridMap message;
      grid_map::GridMapRosConverter::toMessage(grid_map_, {"intensity"}, message);
      grid_map_publisher_->publish(message);
      last_publish_time_ = msg->header.stamp;
    }

    last_time_ = msg->header.stamp;

    if(last_target_scan_time_.is_zero())
      last_target_scan_time_ = last_time_;

    if((last_time_ - last_target_scan_time_).seconds() > 1.0)
    {
      if(!scan_done_.valid() || scan_done_.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready)
      {
        RCLCPP_DEBUG_STREAM(this->get_logger(), "Launching scan");
        scan_done_ = std::async(&MarineRadarTracker::scanForTargets, this, grid_map_);
        last_target_scan_time_ = last_time_;
      }
      else
        RCLCPP_WARN_STREAM(this->get_logger(), "Not ready to scan");
    }
  }

  bool scanForTargets(grid_map::GridMap grid_map)
  {
    grid_map.add("used");
    rclcpp::Time stamp;
    stamp.fromNSec(grid_map.getTimestamp());

    std::vector<std::shared_ptr<Blob> > blobs;
    double resolution = grid_map.getResolution();

    for(grid_map::GridMapIterator it(grid_map); !it.isPastEnd(); ++it)
    {
      if(grid_map.at("intensity", *it) > 0.0 && isnan(grid_map.at("used", *it)))
      {
        std::list<grid_map::Position> to_check;
        grid_map::Position p;
        auto blob = std::make_shared<Blob>(stamp, resolution);
        if(grid_map.getPosition(*it, p))
          to_check.push_back(p);
        while(!to_check.empty())
        {
          p = to_check.front();
          if(grid_map.isInside(p))
            if(!isnan(grid_map.atPosition("intensity", p)))
              if(isnan(grid_map.atPosition("used", p)))
              {
                blob->add(p, grid_map.atPosition("intensity", p));
                grid_map.atPosition("used", p) = 0.0;
                if(grid_map.atPosition("intensity", p) > 0.0)
                {
                  to_check.push_back(grid_map::Position(p.x()+resolution, p.y()));
                  to_check.push_back(grid_map::Position(p.x()-resolution, p.y()));
                  to_check.push_back(grid_map::Position(p.x(), p.y()+resolution));
                  to_check.push_back(grid_map::Position(p.x(), p.y()-resolution));
                }
              }
          to_check.pop_front();
        }
        //ROS_INFO_STREAM("blob: c: " << blob->circularity() << " avg i: " << blob->averageIntensity() << " er: " << blob->effectiveRadius());
        if(!blob->empty())// && blob->effectiveRadius() < 8.0*resolution)
          blobs.push_back(blob);
      }
    }


    for(auto b: blobs)
    {
      project11_msgs::msg::Detect detect;
      detect.header.stamp = b->timestamp();
      detect.header.frame_id = map_frame_;
      detect.sensor_id = sensor_id_;
      detect.pose.pose.orientation.w = 1.0;

      auto center = b->centroid();

      detect.pose.pose.position.x = center.x();
      detect.pose.pose.position.y = center.y();

      // x and y variance
      double variance = pow(b->effectiveRadius()/2.0,2);
      detect.pose.covariance[0] = variance;
      detect.pose.covariance[7] = variance;

      // heading or rotation around z variance
      // unknown so could be off by 180 degrees
      detect.pose.covariance[35] = pow(M_PI,2);

      detect.twist.twist.linear.x = std::nan("");
      detect.twist.twist.linear.y = std::nan("");
      detect.twist.twist.linear.z = std::nan("");
      detect.twist.twist.angular.x = std::nan("");
      detect.twist.twist.angular.y = std::nan("");
      detect.twist.twist.angular.z = std::nan("");

      detects_publisher_->publish(detect);
    }

    visualization_msgs::msg::MarkerArray blob_markers;

    visualization_msgs::msg::Marker marker;
    marker.header.stamp = stamp;
    marker.header.frame_id = map_frame_;
    marker.ns = "radar_blobs";
    marker.action = visualization_msgs::msg::Marker::DELETEALL;
    blob_markers.markers.push_back(marker);

    for(auto b: blobs)
    {
      visualization_msgs::msg::Marker marker;
      marker.header.stamp = b->timestamp();
      marker.header.frame_id = map_frame_;

      marker.ns = "radar_blobs";
      marker.id = blob_markers.markers.size();
      marker.type = marker.SPHERE;
      marker.pose.orientation.w = 1.0;
      marker.color.a = 1.0;
      marker.lifetime.from_seconds(1.0);
      marker.color.r = 1.0;
      marker.color.b = 0.0;
      marker.color.g = 0.0;

      auto center = b->centroid();

      marker.pose.position.x = center.x();
      marker.pose.position.y = center.y();
      marker.pose.position.z = resolution;

      auto radius = b->effectiveRadius();
      marker.scale.x = radius;
      marker.scale.y = radius;
      marker.scale.z = radius;

      blob_markers.markers.push_back(marker);
    }
    markers_publisher_->publish(blob_markers);
    return true;
  }

};

} // namespace marine_radar_tracker

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<marine_radar_tracker::MarineRadarTracker>();

  //marine_radar_tracker::MarineRadarTracker mrt;
    
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}    
*/
