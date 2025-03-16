cmake_minimum_required(VERSION 3.14.4)
project(simrad_halo_radar)

set(ROS_DEPENDS
  ament_cmake
  rclcpp
  marine_radar_control_msgs
  marine_sensor_msgs
  nav_msgs
  rosbag2
  tf2
)

set(EXTERNAL_DEPENDS
)

set(EXTERNAL_DEPENDS_LIB 
)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

###
###  Add dependencies
###

foreach(DEPEND ${ROS_DEPENDS} ${EXTERNAL_DEPENDS})
  find_package(${DEPEND} REQUIRED)
endforeach(DEPEND)

set(SOURCES
    nodes/simrad_halo_radar.cpp
    src/halo_radar.cpp
    src/halo_radar_node.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
ament_target_dependencies(${PROJECT_NAME} ${ROS_DEPENDS} ${EXTERNAL_DEPENDS_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE
   "include/${PROJECT_NAME}/"
)
#target_link_libraries(${PROJECT_NAME}
#  ${ROS_DEPENDS}
#)

#ament_target_dependencies(${PROJECT_NAME}
#  rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)

#target_link_libraries(${PROJECT_NAME} 
#  rclcpp::rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)


#add_executable(fix_angle_increments src/fix_angle_increments.cpp)
#ament_target_dependencies(fix_angle_increments
#  rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)
#target_link_libraries(fix_angle_increments 
#  rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)

#add_executable(fix_scan_times src/fix_scan_times.cpp)
#ament_target_dependencies(fix_scan_times
#  rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)
#target_link_libraries(fix_scan_times
#  rclcpp
#  marine_radar_control_msgs
#  marine_sensor_msgs
#  rosbag2
#)
