cmake_minimum_required(VERSION 3.14.4)
project(marine_radar_layer)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14) #TODO
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
#find_package(std_msgs REQUIRED)
find_package(nav2_costmap_2d REQUIRED)
find_package(pluginlib REQUIRED)
find_package(marine_sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# generate_dynamic_reconfigure_options(cfg/MarineRadarLayer.cfg)

add_library(${PROJECT_NAME} src/marine_radar_layer.cpp)
ament_target_dependencies(${PROJECT_NAME} ${marine_radar_layer_EXPORTED_TARGETS})

target_include_directories(${PROJECT_NAME} PUBLIC
   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
   "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

target_link_libraries(${PROJECT_NAME} PUBLIC
  rclcpp::rclcpp
  ${nav2_costmap_2d_TARGETS}
  ${pluginlib_TARGETS}
  ${marine_sensor_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${tf2_geometry_msgs_TARGETS}
  #${std_msgs_TARGETS}
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib/${PROJECT_NAME}
        ARCHIVE DESTINATION lib/${PROJECT_NAME}
        RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(FILES costmap_plugins.xml
        DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION lib/${PROJECT_NAME}
)

ament_package()
