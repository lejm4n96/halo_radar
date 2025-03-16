#ifndef HALO_RADAR_NODE_HPP
#define HALO_RADAR_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <tf2/utils.h>
#include <iostream>
#include <future>
#include "halo_radar.h"
#include "marine_sensor_msgs/msg/radar_sector.h"
#include "marine_radar_control_msgs/msg/radar_control_set.h"
#include "marine_radar_control_msgs/msg/radar_control_value.h"
#include "nav_msgs/msg/odometry.hpp"
#include "angular_speed_estimator.h"

using std::placeholders::_1;

class RosRadar : public simrad_halo_radar::Radar
{
public: 
    RosRadar(simrad_halo_radar::AddressSet const &addresses); //, rclcpp::Node::SharedPtr node); // : simrad_halo_radar::Radar(addresses);
    
    // Parameters
    //double m_rangeCorrectionFactor_ = this->declare_parameter("~range_correction_factor");
    //std::string m_frame_id;

protected: 
  void processData(std::vector<simrad_halo_radar::Scanline> const &scanlines) override;
  void stateUpdated() override;

private:
    void stateChangeCallback(const marine_radar_control_msgs::RadarControlValue::ConstPtr &cv);
/*    void hbTimerCallback(const ros::TimerEvent &e);
    void createEnumControl(std::string const &name, std::string const &label, std::string const enums[],
                           marine_radar_control_msgs::RadarControlSet &rcs);
    void createFloatControl(std::string const &name, std::string const &label, float min_value, float max_value,
                            marine_radar_control_msgs::RadarControlSet &rcs);
    void createFloatWithAutoControl(std::string const &name, std::string const &auto_name, std::string const &label,
                                    float min_value, float max_value, marine_radar_control_msgs::RadarControlSet &rcs);

*/
    // Pointer to the ROS node 
    /*std::shared_ptr<rclcpp::Node> node_;


    //ros::Publisher m_data_pub;
    rclcpp::Publisher<marine_sensor_msgs::RadarSector>::SharedPtr m_data_pub;
    //ros::Publisher m_state_pub;
    rclcpp::Publisher<marine_radar_control_msgs::RadarControlSet>::sharedPtr m_state_pub;
    //ros::Subscriber m_state_change_sub;
    rclcpp::Subscription<const marine_radar_control_msgs::RadarControlValue::ConstPtr>::SharedPtr m_state_change_sub;
    
    
    //ros::Timer m_heartbeatTimer;

    double m_rangeCorrectionFactor = 1.024;
    std::string m_frame_id = "radar";

    AngularSpeedEstimator m_estimator;
*/
};

#endif