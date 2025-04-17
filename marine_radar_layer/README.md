# marine_radar_layer

A marine radar obstacle layer for nav2_costmap_2d.


## Installation

### Install Dependencies

Run `rosdep update` to install dependencies included in rosdistro.

### Build

In the root of your workspace, run `colcon build`.


## Usage

In your `nav2_params.yaml` file, add "marine_radar_layer" to the `plugins` list:

```plugins: ["marine_radar_layer", ... <other costmap plugins>]```

To initialize the plugin and set its corresponding parameters, add the following in your `nav2_params.yaml`:

```
marine_radar_layer:
    plugin: "marine_radar_layer::MarineRadarLayer"
    enabled: true 
    clear_threshold: 2.0
    mark_threshold: 8.0
    blanking_distance: 4.0
    maximum_intensity: 16.0
```

Todo: bring up nav2 with your `nav2_params.yaml` file.

## Topics and Parameters 

### Subscriptions:

| Subscribed Topic | Data Type                                  | 
|------------------|--------------------------------------------|
| `radar`          | `marine_sensor_msgs/msg/RadarSector`       |

### Parameters:

| Parameter           | Data Type | Default Value |
|---------------------|-----------|---------------|
| `clear_threshold`   | Float     | `2.0`         |
| `mark_threshold`    | Float     | `8.0`         |
| `blanking_distance` | Float     | `4.0`         |
| `maximum_intensity` | Float     | `16.0`        |
