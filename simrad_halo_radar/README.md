# ROS Driver for the Simrad Halo series of marine radars

This driver interfaces with Simrad HALO radar via multicast UDP communication.

## Installation

Clone this repo into your workspace.

Run `rosdep update` to install dependencies included in rosdistro.

Clone the `marine_radar_control_msgs` repo (not included in rosdistro): 

```git clone git@github.com:CCOMJHC/marine_radar_control_msgs.git```


### Optional Packages

Plugin for rqt to view data and control settings: https://github.com/CCOMJHC/rqt_marine_radar

### Build

Run `colcon build` in the root of your workspace to build. 


## Usage

```ros2 run simrad_halo_radar simrad_halo_radar```

By default, the driver will scan all available interfaces. To restrict which interface(s) to use, specify the list of IP local addresses using the `hostIPs` parameter.

The radar starts up in stand-by mode and must be switched into transmit mode before it will produce data. 

### Command-Line Mode Change

To do this from the command line, publish a single `RadarControlValue` message to the `change_state` topic for the corresponding frequency (`/halo_a` or `/halo_b`), thusly:

```ros2 topic pub -1 /halo_a/change_state marine_radar_control_msgs/msg/RadarControlValue "{key: 'status', value: 'transmit'}"```

To switch the radar back into stand-by mode, publish another message with the value `standby`, as follows: 

```ros2 topic pub -1 /halo_a/change_state marine_radar_control_msgs/msg/RadarControlValue "{key: 'status', value: 'standby'}"```

### RQT Plugin Mode Change

Use the `rqt_marine_radar` plugin to switch the radar mode between transmit and standby using the GUI interface in rqt.


## Nodes

### simrad_halo_radar Node 

The `simrad_halo_radar` node publishes data and state from each of the dual frequencies of the Halo radar. The first frequency is addressed by `/halo_a` and `/halo_b`. `<radar_freq_address>` in the topic names below refers to either the `/halo_a` or `/halo_b` topics.

#### Publishers:

| Published Topic              | Data Type                                         |
|------------------------------|---------------------------------------------------|
| `<radar_freq_address>/data`  | `marine_sensor_msgs::msg::RadarSector`            |
| `<radar_freq_address>/state` | `marine_radar_control_msgs::msg::RadarControlSet` |


#### Subscriptions:

| Subscribed Topic                    | Data Type                                           | 
|-------------------------------------|-----------------------------------------------------|
| `<radar_freq_address>/change_state` | `marine_radar_control_msgs::msg::RadarControlValue` |


#### Parameters:

| Parameter                                      | Data Type     | Default Value |
|------------------------------------------------|---------------|---------------|
| `<radar_freq_address>.frame_id`                | `std::string` | `"radar"`     |
| `<radar_freq_address>.range_correction_factor` | `double`      | `1.024`       |


#### Radar State Parameters:

The following parameters are used to control the radar. They can be set either via the command line (as example above) or via the `rqt_marine_radar` plugin. The radar state is published on the `<radar_freq_address>/state` via a `RadarControlSet` message.

| Parameter               | Description                 | Values                                            | 
|-------------------------|-----------------------------|---------------------------------------------------|
| `status`                | Radar state                 | `standby`, `transmit`                             |
| `range`                 | Radar range [meters]        | `25 m`-`75,000 m`                                 |
| `mode`                  | Radar mode                  | `custom`, `harbor`, `offshore`, `weather`, `bird` |
| `gain`                  | Radar gain                  | `0`-`100`                                         |
| `sea_clutter`           | Sea clutter mode            | `0`-`100`                                         |
| `rain_clutter`          | Rain clutter mode           | `0`-`100`                                         |
| `noise_rejection`       | Noise rejection             | `off`, `low`, `medium`, `high`                    |
| `target_expansion`      | Target expansion            | `off`, `low`, `medium`, `high`                    |
| `inteference_rejection` | Interference rejection      | `off`, `low`, `medium`, `high`                    |
| `target_separation`     | Target separation           | `off`, `low`, `medium`, `high`                    |
| `scan_speed`            | Scan speed                  | `off`, `medium`, `high`                           |
| `doppler_mode`          | Doppler mode                | `off`, `normal`, `approaching_only`               |
| `doppler_speed`         | Doppler speed               | `0.5`-`15.95`                                     |
| `antenna_height`        | Antenna height [meters]     | `0.0 m`-`30.175 m`                                |
| `bearing_alignment`     | Bearing alignment [degrees] | `0.0&deg;`-`360.0&deg;`                           |
| `sidelobe_suppression`  | Sidelobe suppression mode   | `0`-`100`                                         |
| `lights`                | Halo lights                 | `off`, `low`, `medium`, `high`                    |


## Troubleshooting

To make sure route is available: 

```sudo route add -net 224.0.0.0 netmask 224.0.0.0 eth0```

Switches and routers between the radar and the machine running the driver may interfere with multicast packets. Consult network equipment documentation or simply the network path.
