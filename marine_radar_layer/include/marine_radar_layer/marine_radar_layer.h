#ifndef MARINE_RADAR_LAYER_H
#define MARINE_RADAR_LAYER_H

#include <rclcpp/rclcpp.hpp>
#include <nav2_costmap_2d/layer.hpp>
#include <nav2_costmap_2d/layered_costmap.hpp>
#include <marine_sensor_msgs/msg/radar_sector.hpp>

using std::placeholders::_1;

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

  // Pure virtual functions in nav2_costmap_2d that must be implemented by inheriting classes
  // Declaring functions here but currently provide no implementation
  void reset();
  bool isClearable();

  virtual void matchSize() override;

  /**
   * @brief Structure that corresponds to the parameters advertised by the 
   * MarineRadarLayer class. 
   */
  struct Parameters
  {
    float m_clear_threshold;
    float m_mark_threshold;
    float m_blanking_distance;
    float m_maximum_intensity;
    
    /**
     * @brief declares all the parameters and initializes all the stored variables
     * within the struct
     * @param node  A pointer or reference to the node you want to use to
     * initialize the parameters.
     */
    void init(rclcpp::Node * node);
  };

  /**
   * @brief gets a reference to the MarineRadarLayer::Parameters
   * @return the parameters associated with the node
   */
  const Parameters & getParams(){return parameters_;}

protected:
  Parameters parameters_;

private:
  //void reconfigureCallback(MarineRadarLayerConfig &config, uint32_t level);

  //typedef dynamic_reconfigure::Server<MarineRadarLayerConfig> ReconfigureServer;
  //typedef std::shared_ptr<ReconfigureServer> ReconfigureServerPtr;
  //ReconfigureServerPtr m_reconfigureServer;

  void radarSectorCallback(const marine_sensor_msgs::msg::RadarSector::ConstSharedPtr &msg);

  std::list<marine_sensor_msgs::msg::RadarSector::ConstSharedPtr> m_sector_buffer;
  std::mutex m_sector_buffer_mutex;

  struct PositionedSector
  {
    bool valid_position = false;
    double yaw;
    double x;
    double y;
    marine_sensor_msgs::msg::RadarSector::ConstSharedPtr sector;
    float getValue(double x, double y, double blanking_distance=0.0);
  };

  std::map<double, PositionedSector> m_sectors;

  std::string m_global_frame_id;

  rclcpp::Subscription<marine_sensor_msgs::msg::RadarSector>::ConstSharedPtr m_radar_subscriber;

  double m_last_range = 0.0; // use to detect radar range change
};

} // namespace marine_radar_layer

#endif
