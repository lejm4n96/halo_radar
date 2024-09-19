#!/usr/bin/env python3

import rospy
from marine_radar_control_msgs.msg import RadarControlValue

class StatusChangerNode(object):
    def __init__(self, config):
        self.node_name = rospy.get_name()
        self.config = config

        # publisher
        self.pub = rospy.Publisher('~topic', RadarControlValue, queue_size=1)
        rospy.sleep(1)

        # Change the status
        self.change_status()

    def change_status(self):
        msg = RadarControlValue()
        # Create an publish the message
        for key, value in self.config.items():
            msg.key = key
            msg.value = str(value)
            self.pub.publish(msg)
            rospy.loginfo(f"Published {key}: {value} on {self.pub.resolved_name}")

if __name__ == '__main__':
    try:
        rospy.init_node(StatusChangerNode.__name__)
        config = rospy.get_param('~')
        n = StatusChangerNode(config)
        rospy.spin()
    except rospy.ROSInterruptException:
        pass