#!/usr/bin/env python3

import rospy
from marine_radar_control_msgs.msg import RadarControlValue

class StatusChangerNode(object):
    def __init__(self):
        self.node_name = rospy.get_name()

        # publisher
        self.pub = rospy.Publisher('~topic', RadarControlValue, queue_size=1)
        rospy.sleep(1)

        self.status = rospy.get_param("~status")

        # Change the status
        self.change_status()
        rospy.loginfo(f"Changed status to {self.status} and node shutting down")
        rospy.signal_shutdown("")

    def change_status(self):
        # Toggle the status
        
        # Create an publish the message
        msg = RadarControlValue()
        msg.key = 'status'
        msg.value = self.status

        self.pub.publish(msg)

if __name__ == '__main__':
    try:
        rospy.init_node(StatusChangerNode.__name__)
        n = StatusChangerNode()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass