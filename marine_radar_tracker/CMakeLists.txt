cmake_minimum_required(VERSION 3.8)
project(marine_radar_tracker)

set(ROS_DEPENDS
    geometry_msgs
    grid_map_ros
    marine_sensor_msgs
    pcl_ros
    project11_msgs
    rclcpp
    rosbag2
    tf2
    tf2_ros
    visualization_msgs
    std_msgs #TODO for testing, remove
)

set(EXTERNAL_DEPENDS
)

set(EXTERNAL_DEPENDS_LIB 
)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

foreach(DEPEND ${ROS_DEPENDS} ${EXTERNAL_DEPENDS})
  find_package(${DEPEND} REQUIRED)
endforeach(DEPEND)

add_executable(${PROJECT_NAME} src/marine_radar_tracker.cpp)
target_include_directories(${PROJECT_NAME}
    PRIVATE 
    "include/${PROJECT_NAME}/"
)
ament_target_dependencies(${PROJECT_NAME}
    ${ROS_DEPENDS}
    ${EXTERNAL_DEPENDS_LIB}
)

#add_executable(${PROJECT_NAME} src/marine_radar_tracker.cpp)
#add_dependencies(${PROJECT_NAME} ${marine_radar_tracker_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

#add_executable(marine_radar_to_pointcloud src/marine_radar_to_pointcloud.cpp)
#add_dependencies(marine_radar_to_pointcloud ${marine_radar_tracker_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(marine_radar_to_pointcloud ${catkin_LIBRARIES})

#install(TARGETS ${PROJECT_NAME} marine_radar_to_pointcloud
#        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
#)
