#include "halo_radar_node.hpp"


std::shared_ptr<simrad_halo_radar::HeadingSender> headingSender;


// This function appears to be unsued, so commenting out for now
/*void odometryCallback(const nav_msgs::Odometry::ConstPtr msg)
{
  if(headingSender)
  {
    double heading = 90.0-180.0*tf2::getYaw(msg->pose.pose.orientation)/M_PI;
    headingSender->setHeading(heading);
  }
    
}*/


int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::Node::SharedPtr node = std::make_shared<rclcpp::Node>("simrad_halo_radar");
  
  std::vector<std::shared_ptr<RosRadar> > radars;
  std::vector<uint32_t> hostIPs;

  std::vector<std::string> hostIPstrings;
  node->declare_parameter("hostIPs", hostIPstrings);
  node->get_parameter("hostIPs", hostIPstrings);

  for (auto s: hostIPstrings)
    hostIPs.push_back(simrad_halo_radar::ipAddressFromString(s));

  while(radars.empty())
  {
    std::vector<simrad_halo_radar::AddressSet> as;
    if(hostIPs.empty())
      as = simrad_halo_radar::scan();
    else
      as = simrad_halo_radar::scan(hostIPs);
    if(as.empty())
      RCLCPP_WARN_STREAM(node->get_logger(), "No radars found!");
    for (auto a : as)
    {
      radars.push_back(std::shared_ptr<RosRadar>(new RosRadar(node, a)));
      // if(!headingSender)
      //   headingSender = std::shared_ptr<simrad_halo_radar::HeadingSender>(new simrad_halo_radar::HeadingSender(a.interface));
    }
  }

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
