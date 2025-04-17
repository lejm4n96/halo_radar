# ROS2 Marine Radar Packages

This repository contains ROS2-ported packages for working with marine radars, including radar drivers and packages for processing radar data for vehicle navigation. 

![](./docs/media/marine_radar_tracker/radar_pointcloud.gif)
*Simrad Halo radar data visualized in Rviz*

## Message Packages

### marine_radar_control_msgs

Defines messages for use by marine radar drivers to control radars. See the [marine_radar_control_msgs README](./marine_radar_control_msgs/README.md) for details.


## Driver Packages

### simrad_halo_radar

Driver for Simrad Halo series of marine radars. See the [simrad_halo_radar README](./simrad_halo_radar/README.md) for details.


## Processing Packages

### marine_radar_layer

Defines a `nav2_costmap_2d` layer for marine radars for use with the ROS2 `nav2` navigation stack. See [marine_radar_layer README](./marine_radar_layer/README.md) for details.

### marine_radar_tracker

Contains nodes for publishing radar data as a ROS pointcloud2 for easy display in Rviz. Radar tracker nodes are not ported to ROS2 and development is still in-progress. See [marine_radar_tracker README](./marine_radar_tracker/README.md) for details.


## You Might Also Like

### rqt_marine_radar

Plugin for visualizing marine radar data and controlling radar via RQT GUI. See the [rqt_marine_radar](https://github.com/CCOMJHC/rqt_marine_radar) repository for installation and usage details.