#include "halo_radar_node.hpp"


class SimradHaloRadarNode : public rclcpp::Node
{
public:
    SimradHaloRadarNode() : Node("simrad_halo_radar")
    {
        //this->declare_parameter("~hostIPs", rclcpp::PARAMETER_STRING);
    }
};

std::shared_ptr<simrad_halo_radar::HeadingSender> headingSender;

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
  //auto node = rclcpp::Node::make_shared("simrad_halo_radar");
  rclcpp::Node::SharedPtr node = std::make_shared<SimradHaloRadarNode>();
  
  std::vector<std::shared_ptr<RosRadar> > radars;
  std::vector<uint32_t> hostIPs;

  std::vector<std::string> hostIPstrings;
  node->declare_parameter("hostIPs", hostIPstrings);
  node->get_parameter("hostIPs", hostIPstrings);

  //ros::param::get("~hostIPs", hostIPstrings);
  // this only handles one!  what if there are multiple radars?
  // seems like before it returned a vector of strings, but now it just returns
  // an rclcpp::Parameter...? can that hold multiple parameters? I'm so confused...



  // hostIPstrings.push_back(node->get_parameter("~hostIPs").as_string());
  for (auto s: hostIPstrings)
    hostIPs.push_back(simrad_halo_radar::ipAddressFromString(s));

  std::future<void> scanResult = std::async(std::launch::async, [&] {
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
        if(!headingSender)
          headingSender = std::shared_ptr<simrad_halo_radar::HeadingSender>(new simrad_halo_radar::HeadingSender(a.interface));
      }
    }
  });

  rclcpp::spin(node); 
  rclcpp::shutdown();
  return 0;
}
