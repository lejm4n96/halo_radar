# marine_radar_layer

A marine radar obstacle layer for nav2_costmap_2d.


## Parameters

| Parameter           | Description                                 | Type  | 
|---------------------|---------------------------------------------|-------|
| `clear_threshold`   |                                             | Float |
| `mark_threshold`    |                                             | Float |
| `blanking_distance` |                                             | Float |
| `maximum_intensity` |                                             | Float |


## Topics 

### Subscribed Topics 

| Topic   | Data Type                                  | 
|---------|--------------------------------------------|
| `radar` | `marine_sensor_msgs/RadarSector`           |