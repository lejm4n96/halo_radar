#ifndef HALO_RADAR_NODE_HPP
#define HALO_RADAR_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <tf2/utils.h>
#include <iostream>
#include <future>
#include "halo_radar.h"
#include "marine_sensor_msgs/msg/radar_sector.hpp"
#include "marine_sensor_msgs/msg/radar_echo.hpp"
#include "marine_radar_control_msgs/msg/radar_control_set.hpp"
#include "marine_radar_control_msgs/msg/radar_control_value.hpp"
#include "marine_radar_control_msgs/msg/radar_control_item.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "angular_speed_estimator.h"

using std::placeholders::_1;

class RosRadar : public simrad_halo_radar::Radar
{
public:

  /*!
   * \brief RosRadar  a brief description of the constructor
   * \param addresses this is what the address does
   */
  RosRadar(rclcpp::Node::SharedPtr node, simrad_halo_radar::AddressSet const &addresses);

protected: 
  void processData(std::vector<simrad_halo_radar::Scanline> const &scanlines) override;
  void stateUpdated() override;

private:
  void stateChangeCallback(const marine_radar_control_msgs::msg::RadarControlValue::SharedPtr cv);
  void hbTimerCallback();
  void createEnumControl(std::string const &name, std::string const &label, std::string const enums[],
                           marine_radar_control_msgs::msg::RadarControlSet &rcs);
  void createFloatControl(std::string const &name, std::string const &label, float min_value, float max_value,
                            marine_radar_control_msgs::msg::RadarControlSet &rcs);
  void createFloatWithAutoControl(std::string const &name, std::string const &auto_name, std::string const &label,
                                    float min_value, float max_value, marine_radar_control_msgs::msg::RadarControlSet &rcs);

    // Pointer to the ROS node 
    rclcpp::Node::SharedPtr node_;
    
    rclcpp::Publisher<marine_sensor_msgs::msg::RadarSector>::SharedPtr m_data_pub;
    rclcpp::Publisher<marine_radar_control_msgs::msg::RadarControlSet>::SharedPtr m_state_pub;
    rclcpp::Subscription<marine_radar_control_msgs::msg::RadarControlValue>::SharedPtr m_state_change_sub;
    
    rclcpp::TimerBase::SharedPtr m_heartbeatTimer;
    
    double m_rangeCorrectionFactor = 1.024;
    std::string m_frame_id = "radar";

    AngularSpeedEstimator m_estimator;

};

#endif
