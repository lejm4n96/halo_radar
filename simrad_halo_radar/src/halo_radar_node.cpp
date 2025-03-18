#include "halo_radar_node.hpp"

RosRadar::RosRadar(rclcpp::Node::SharedPtr node, simrad_halo_radar::AddressSet const &addresses)
: simrad_halo_radar::Radar(addresses)
{

    this->node_ = node;

    // Check whether node is valid before proceeding
    //if (rcl::rcl_node_is_valid(&node))
    
    node_->declare_parameter(addresses.label + ".range_correction_factor", this->m_rangeCorrectionFactor);
    node_->get_parameter(addresses.label + ".range_correction_factor",this->m_rangeCorrectionFactor);

    node_->declare_parameter(addresses.label + ".frame_id", this->m_frame_id);
    node_->get_parameter(addresses.label + ".frame_id",this->m_frame_id);


    this->m_data_pub = node_->create_publisher<marine_sensor_msgs::msg::RadarSector>(addresses.label + "/data", 10);
    this->m_state_pub = node_->create_publisher<marine_radar_control_msgs::msg::RadarControlSet>(addresses.label + "/state", 10);
    this->m_state_change_sub = node_->create_subscription<marine_radar_control_msgs::msg::RadarControlValue>(
                               addresses.label + "/change_state", 10, std::bind(&RosRadar::stateChangeCallback, this, _1));

    m_heartbeatTimer = node_->create_wall_timer(std::chrono::seconds(1),
                        std::bind(&RosRadar::hbTimerCallback, this));

    startThreads();

    m_radar_id = addresses.label;
    RCLCPP_INFO(node_->get_logger(), "%s subnode started",m_radar_id.c_str());
}

RosRadar::~RosRadar()
{
  RCLCPP_INFO(node_->get_logger(), "%s subnode started",m_radar_id.c_str());

}

void RosRadar::processData(std::vector<simrad_halo_radar::Scanline> const &scanlines)
{
  if(scanlines.empty())
    return;
  marine_sensor_msgs::msg::RadarSector rs;
  rs.header.stamp = this->node_->get_clock()->now();
  rs.header.frame_id = m_frame_id;
  rs.angle_start = 2.0*M_PI*(360-scanlines.front().angle)/360.0;
  double  angle_max = 2.0*M_PI*(360-scanlines.back().angle)/360.0;
  if(scanlines.size() > 1)
  {
    if (angle_max > rs.angle_start && angle_max-rs.angle_start > M_PI) // have we looped around (also make sure angle are decreasing)
      angle_max -= 2.0*M_PI;
    rs.angle_increment = (angle_max-rs.angle_start)/double(scanlines.size()-1);

  }
  rs.range_min = 0.0;
  rs.range_max = scanlines.front().range;
  for (auto sl : scanlines)
  {
    marine_sensor_msgs::msg::RadarEcho echo;
    for (auto i : sl.intensities)
      echo.echoes.push_back(i/15.0); // 4 bit int to float
    rs.intensities.push_back(echo);
  }

  auto angular_speed = m_estimator.update(rs.header.stamp,  rs.angle_start);
  double scan_time = 0.0;
  if(angular_speed != 0.0)
    scan_time = 2*M_PI/fabs(angular_speed);

  rs.scan_time = rclcpp::Duration::from_seconds(scan_time);

  double time_increment = 0.0;
  if (scan_time > 0)
    time_increment = std::abs(rs.angle_increment)/scan_time;
  rs.time_increment =  rclcpp::Duration::from_seconds(time_increment);

  this->m_data_pub->publish(rs);
}

void RosRadar::stateUpdated()
{
  marine_radar_control_msgs::msg::RadarControlSet rcs;

  std::string statusEnums[] = {"standby", "transmit", ""};

  createEnumControl("status", "Status", statusEnums, rcs);
  createFloatControl("range", "Range", 25, 75000, rcs);

  std::string modeEnums[] = {"custom", "harbor", "offshore", "weather", "bird", ""};

  createEnumControl("mode", "Mode", modeEnums, rcs);
  createFloatWithAutoControl("gain", "gain_mode", "Gain", 0, 100, rcs);
  createFloatWithAutoControl("sea_clutter", "sea_clutter_mode", "Sea clutter", 0, 100, rcs);
  createFloatControl("auto_sea_clutter_nudge", "Auto sea clut adj", -50, 50, rcs);

  std::string seaStateEnums[] = {"calm", "moderate", "rough", ""};

  createEnumControl("sea_state", "Sea state", seaStateEnums, rcs);
  createFloatControl("rain_clutter", "Rain clutter", 0, 100, rcs);

  std::string lowMedHighEnums[] = {"off", "low", "medium", "high", ""};

  createEnumControl("noise_rejection", "Noise rejection", lowMedHighEnums, rcs);
  createEnumControl("target_expansion", "Target expansion", lowMedHighEnums, rcs);
  createEnumControl("interference_rejection", "Interf. rej", lowMedHighEnums, rcs);
  createEnumControl("target_separation", "Target separation", lowMedHighEnums, rcs);

  std::string scanSpeedEnums[] = {"off", "medium", "high", ""};

  createEnumControl("scan_speed", "Fast scan", scanSpeedEnums, rcs);

  std::string dopplerModeEnums[] = {"off", "normal", "approaching_only", ""};

  createEnumControl("doppler_mode", "VelocityTrack", dopplerModeEnums, rcs);
  createFloatControl("doppler_speed", "Speed threshold", 0.05, 15.95, rcs);
  createFloatControl("antenna_height", "Antenna height", 0.0, 30.175, rcs);
  createFloatControl("bearing_alignment", "Bearing alignment", 0, 360, rcs);
  createFloatWithAutoControl("sidelobe_suppression", "sidelobe_suppression_mode", "Sidelobe sup.", 0, 100, rcs);
  createEnumControl("lights", "Halo light", lowMedHighEnums, rcs);

  this->m_state_pub->publish(rcs);
}

void RosRadar::stateChangeCallback(const marine_radar_control_msgs::msg::RadarControlValue::SharedPtr cv)
{
  sendCommand(cv->key, cv->value);
}

void RosRadar::hbTimerCallback()
{
  if (checkHeartbeat())
    stateUpdated();
}

void RosRadar::createEnumControl(std::string const &name, std::string const &label, std::string const enums[],
                                 marine_radar_control_msgs::msg::RadarControlSet &rcs)
{
  if (m_state.find(name) != m_state.end())
  {
    marine_radar_control_msgs::msg::RadarControlItem rci;
    rci.name = name;
    rci.value = m_state[name];
    rci.label = label;
    rci.type = marine_radar_control_msgs::msg::RadarControlItem::CONTROL_TYPE_ENUM;
    for (int i = 0; !enums[i].empty(); i++)
      rci.enums.push_back(enums[i]);
    rcs.items.push_back(rci);
  }
}

void RosRadar::createFloatControl(std::string const &name, std::string const &label, float min_value, float max_value,
                                  marine_radar_control_msgs::msg::RadarControlSet &rcs)
{
  if (m_state.find(name) != m_state.end())
  {
    marine_radar_control_msgs::msg::RadarControlItem rci;
    rci.name = name;
    rci.value = m_state[name];
    rci.label = label;
    rci.type = marine_radar_control_msgs::msg::RadarControlItem::CONTROL_TYPE_FLOAT;
    rci.min_value = min_value;
    rci.max_value = max_value;
    rcs.items.push_back(rci);
  }
}

void RosRadar::createFloatWithAutoControl(std::string const &name, std::string const &auto_name, std::string const &label,
                                          float min_value, float max_value, marine_radar_control_msgs::msg::RadarControlSet &rcs)
{
  if (m_state.find(name) != m_state.end() && m_state.find(auto_name) != m_state.end())
  {
    marine_radar_control_msgs::msg::RadarControlItem rci;
    rci.name = name;
    std::string value = m_state[name];
    if (m_state[auto_name] == "auto")
      value = "auto";
    rci.value = value;
    rci.label = label;
    rci.type = marine_radar_control_msgs::msg::RadarControlItem::CONTROL_TYPE_FLOAT_WITH_AUTO;
    rci.min_value = min_value;
    rci.max_value = max_value;
    rcs.items.push_back(rci);
  }
}
