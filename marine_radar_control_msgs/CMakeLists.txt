cmake_minimum_required(VERSION 3.8)
project(marine_radar_control_msgs)

set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

set(msg_files 
  msg/RadarControlItem.msg
  msg/RadarControlSet.msg
  msg/RadarControlValue.msg
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES
)

ament_package()
