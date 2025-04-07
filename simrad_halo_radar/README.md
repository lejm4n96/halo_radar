# ROS Driver for the Simrad Halo series of marine radars

This driver interfaces with Simrad HALO radar via multicast UDP communication.

## Installation

Clone this repo into your workspace.

Run `rosdep update` to install dependencies included in rosdistro.

### Optional packages

Plugin for rqt to view data and control settings: https://github.com/CCOMJHC/rqt_marine_radar

### Build

Run `colcon build` in the root of your workspace to build. 

## Usage

```ros2 run simrad_halo_radar simrad_halo_radar```

By default, the driver will scan all available interfaces. To restrict which interface(s) to use, specify the list of IP local addresses using the `hostIPs` parameter.

The radar starts up in stand-by mode and must be switched into transmit mode before it will produce data. 

To do this from the command line, publish a single `RadarControlValue` message to the `change_state` topic for the corresponding frequency (`/halo_a` or `/halo_b`), thusly:

```ros2 topic pub -1 /halo_a/change_state marine_radar_control_msgs/msg/RadarControlValue "{key: 'status', value: 'transmit'}"```

To switch the radar back into stand-by mode, publish another message with the value `standby`, as follows: 

```ros2 topic pub -1 /halo_a/change_state marine_radar_control_msgs/msg/RadarControlValue "{key: 'status', value: 'standby'}"```

If using the `rqt_marine_radar` plugin, the radar mode can be switched in the rqt GUI. 

## Nodes

### simrad_halo_radar Node 

#### Parameters:

#### Publishers:


#### Subscriptions:


| Topic   | Data Type                                  | 
|---------|--------------------------------------------|
| `todo` | `todo`           |

## Troubleshooting

To make sure route is available: 

```sudo route add -net 224.0.0.0 netmask 224.0.0.0 eth0```

Switches and routers between the radar and the machine running the driver may interfere with multicast packets. Consult network equipment documentation or simply the network path.
