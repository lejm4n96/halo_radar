#ifndef MARINE_RADAR_LAYER_H
#define MARINE_RADAR_LAYER_H

#include <rclcpp/rclcpp.hpp>
#include <nav2_costmap_2d/layer.hpp>
#include <nav2_costmap_2d/layered_costmap.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>

namespace marine_radar_layer
{

class MarineRadarLayer: public nav2_costmap_2d::Layer, public nav2_costmap_2d::Costmap2D, public rclcpp::Node
{
public:
  MarineRadarLayer();

  virtual void onInitialize() override;
  virtual void updateBounds(double robot_x, double robot_y, double robot_yaw, double* min_x, double* min_y, double* max_x, double* max_y) override;
  virtual void updateCosts(nav2_costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j)  override;

  bool isDiscretized();

  virtual void matchSize() override;

private:
  //void reconfigureCallback(MarineRadarLayerConfig &config, uint32_t level);

  //typedef dynamic_reconfigure::Server<MarineRadarLayerConfig> ReconfigureServer;
  //typedef std::shared_ptr<ReconfigureServer> ReconfigureServerPtr;
  //ReconfigureServerPtr m_reconfigureServer;

  //OnSetParametersCallbackHandle::SharedPtr set_param_res_;
  //rcl_interfaces::msg::SetParametersResult paramCallback(const std::vector<rclcpp::Parameter> & parameters);
  //void declareMarineRadarLayerParameters();


  void radarSectorCallback(const marine_sensor_msgs::RadarSectorConstPtr &msg);

  std::list<marine_sensor_msgs::RadarSectorConstPtr> m_sector_buffer;
  std::mutex m_sector_buffer_mutex;

  struct PositionedSector
  {
    bool valid_position = false;
    double yaw;
    double x;
    double y;
    marine_sensor_msgs::RadarSectorConstPtr sector;
    float getValue(double x, double y, double blanking_distance=0.0);
  };

  std::map<double, PositionedSector> m_sectors;

  std::string m_global_frame_id;

  //ros::Subscriber m_radar_subscriber;
  rclcpp::Subscription m_radar_subscriber;

  //struct MarineRadarLayerParam
  //{
    float m_clear_threshold;
    float m_mark_threshold;
    float m_blanking_distance;
    float m_maximum_intensity;
  //} marine_radar_layer_param;

  double m_last_range = 0.0; // use to detect radar range change
};

} // namespace marine_radar_layer

#endif
