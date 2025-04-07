# Convert marine radar data to grid_maps and track targets

## Installation

### Install Dependencies

Run `rosdep update` to install dependencies included in rosdistro.

Clone `project11_msgs` (not included in rosdistro): 

```git clone https://github.com/CCOMJHC/project11_msgs```

### Build

In the root of your workspace, run `colcon build`.

## Nodes

### marine_radar_to_pointcloud Node

#### Publishers:

| Published Topics | Data Type |
|------------------|-----------|
| `pointcloud`     | `sensor_msgs/PointCloud2` |

#### Subscribers:

| Subscribed Topics | Data Type |
|------------------|-----------|
| `radar_data`     | `marine_sensor_msgs/RadarSector` |



### marine_radar_tracker Node 

Not yet ported