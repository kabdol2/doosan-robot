#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ##
# @brief    [py example basic] basic test for doosan robot
# @author   Kab Kyoum Kim (kabkyoum.kim@doosan.com)   

import rospy
import os
import threading, time
from std_msgs.msg import String,Int32,Int32MultiArray,Float64,Float64MultiArray,MultiArrayLayout,MultiArrayDimension
import sys
sys.dont_write_bytecode = True
#sys.path.append( os.path.abspath(os.path.join(os.path.dirname(__file__),"../../../../common/imp")) ) # get import pass : DSR_ROBOT.py 
#from DSR_ROBOT import *

from dsr_msgs.msg import *
from dsr_msgs.srv import *

ROBOT_ID     = "dsr01"
ROBOT_MODEL  = "m1013"

def SET_ROBOT(id, model):
    ROBOT_ID = id; ROBOT_MODEL= model   

def shutdown():
    print "shutdown time!"
    print "shutdown time!"
    print "shutdown time!"

    pub_stop.publish(stop_mode=1) #STOP_TYPE_QUICK)
    return 0

# convert list to Float64MultiArray
def _ros_listToFloat64MultiArray(list_src):
    _res = []
    for i in list_src:
        item = Float64MultiArray()
        item.data = i
        _res.append(item)
    #print(_res)
    #print(len(_res))
    return _res
 
if __name__ == "__main__":
    #----- set target robot --------------- 
    my_robot_id    = "dsr01"
    my_robot_model = "m1013"
    SET_ROBOT(my_robot_id, my_robot_model)

    rospy.init_node('dsr_service_io_basic_py')
    rospy.on_shutdown(shutdown)


    pub_stop = rospy.Publisher('/'+ROBOT_ID +ROBOT_MODEL+'/stop', RobotStop, queue_size=10)           
    
    #print 'wait services'
    #rospy.wait_for_service('/'+ROBOT_ID +ROBOT_MODEL+'/drl/drl_start')

    set_digital_output = rospy.ServiceProxy('/' + ROBOT_ID + ROBOT_MODEL + '/io/set_digital_output', SetCtrlBoxDigitalOutput)
    get_digital_input = rospy.ServiceProxy('/' + ROBOT_ID + ROBOT_MODEL + '/io/get_digital_input', GetCtrlBoxDigitalInput)

    set_digital_output(2, True)
    if get_digital_input(2) == 1 :
        set_digital_output(11, True)
    
    
    while not rospy.is_shutdown():
        pass

    print 'good bye!'
