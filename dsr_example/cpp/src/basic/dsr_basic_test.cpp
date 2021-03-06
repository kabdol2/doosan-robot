/*
 * [c++ example basic] basic test for doosan robot
 * Author: Kab Kyoum Kim (kabkyoum.kim@doosan.com)
 *
 * Copyright (c) 2019 Doosan Robotics
 * Use of this source code is governed by the BSD, see LICENSE
*/

#include <ros/ros.h>
#include <signal.h>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <array>
#include <vector>
#include <algorithm>

#include <dsr_msgs/RobotState.h>
#include <dsr_msgs/RobotStop.h>

#include <dsr_msgs/MoveJoint.h>
#include <dsr_msgs/MoveLine.h>
#include <dsr_msgs/MoveJointx.h>
#include <dsr_msgs/MoveCircle.h>
#include <dsr_msgs/MoveSplineJoint.h>
#include <dsr_msgs/MoveSplineTask.h>
#include <dsr_msgs/MoveBlending.h>
#include <dsr_msgs/MoveSpiral.h>
#include <dsr_msgs/MovePeriodic.h>
#include <dsr_msgs/MoveWait.h>

#include "dsr_util.h"
#include "dsr_robot.h"

using namespace std;

//----- set tartget robot----------------------------------------------------
string ROBOT_ID     = "dsr01";
string ROBOT_MODEL  = "m1013";
void SET_ROBOT(string id, string model) {ROBOT_ID = id; ROBOT_MODEL= model;}   
//---------------------------------------------------------------------------

int movej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 0)             
{     
    //ros::ServiceClient srvMoveJoint = nh.serviceClient<dsr_msgs::MoveJoint>("/dsr01m1013/motion/move_joint");
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();
    ros::ServiceClient srvMoveJoint = node->serviceClient<dsr_msgs::MoveJoint>("/"+ROBOT_ID + ROBOT_MODEL+"/motion/move_joint");

    dsr_msgs::MoveJoint srv;

    for(int i=0; i<NUM_JOINT; i++)
        srv.request.pos[i] = fTargetPos[i];
    srv.request.vel = fTargetVel;
    srv.request.acc = fTargetAcc;
    srv.request.time = fTargetTime;
    srv.request.radius = fBlendingRadius;
    srv.request.mode = nMoveMode;
    srv.request.blendType = nBlendingType; 
    srv.request.syncType = nSyncType;

    //ROS_INFO("service call: /dsr01m1013/motion/move_joint");
    //ROS_INFO("  <pos> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",srv.request.pos[0],srv.request.pos[1],srv.request.pos[2],srv.request.pos[3],srv.request.pos[4],srv.request.pos[5]);
    //ROS_INFO("  <vel> %7.3f , <acc> %7.3f, <time> %7.3f",srv.request.vel, srv.request.acc, srv.request.time);
    //ROS_INFO("  <mode> %d , <radius> %7.3f, <blendType> %d",srv.request.mode, srv.request.radius, srv.request.blendType);

    if(srvMoveJoint.call(srv))
    {         
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {    
        ROS_ERROR("Failed to call service dr_control_service : move_joint\n");
        ros::shutdown();  
        return -1;
    }
}
int amovej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 1)             
{
    return movej(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, fBlendingRadius, nMoveMode, nBlendingType, /*nSyncType=*/ 1);             
}


int movel(float fTargetPos[NUM_JOINT], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 0)             
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveLine = node->serviceClient<dsr_msgs::MoveLine>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_line");
    dsr_msgs::MoveLine srv;

    for(int i=0; i<NUM_JOINT; i++)
        srv.request.pos[i] = fTargetPos[i];
    for(int i=0; i<2; i++){
        srv.request.vel[i] = fTargetVel[i];
        srv.request.acc[i] = fTargetAcc[i];
    }
    srv.request.time = fTargetTime;
    srv.request.radius = fBlendingRadius;
    srv.request.ref  = nMoveReference;
    srv.request.mode = nMoveMode;
    srv.request.blendType = nBlendingType; 
    srv.request.syncType = nSyncType;

    ROS_INFO("service call: /dsr01m1013/motion/move_line");
    ROS_INFO("  <pos> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",srv.request.pos[0],srv.request.pos[1],srv.request.pos[2],srv.request.pos[3],srv.request.pos[4],srv.request.pos[5]);
    ROS_INFO("  <vel> %7.3f,%7.3f <acc> %7.3f,%7.3f <time> %7.3f",srv.request.vel[0],srv.request.vel[1],srv.request.acc[0],srv.request.acc[1], srv.request.time);
    ROS_INFO("  <mode> %d, <ref> %d, <radius> %7.3f, <blendType> %d",srv.request.mode,srv.request.ref, srv.request.radius, srv.request.blendType);

    if(srvMoveLine.call(srv))
    {         
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {     
        ROS_ERROR("Failed to call service dr_control_service : move_line\n");
        ros::shutdown();
        return -1;
    }
}
int amovel(float fTargetPos[NUM_JOINT], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 1)             
{
    return movel(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, fBlendingRadius, nMoveReference, nMoveMode, nBlendingType, /*nSyncType=*/ 1);
}


int movejx(float fTargetPos[NUM_TASK], float fTargetVel, float fTargetAcc, float fTargetTime=0.f, float fBlendingRadius = 0.f,
           int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSolSpace = 0, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveJointx = node->serviceClient<dsr_msgs::MoveJointx>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_jointx");
    dsr_msgs::MoveJointx srv; 

    for(int i=0; i<NUM_TASK; i++)
        srv.request.pos[i] = fTargetPos[i];
    srv.request.vel = fTargetVel;
    srv.request.acc = fTargetAcc;
    srv.request.time = fTargetTime;
    srv.request.radius = fBlendingRadius;
    srv.request.ref = nMoveReference;
    srv.request.mode = nMoveMode;
    srv.request.blendType = nBlendingType;
    srv.request.sol = nSolSpace;

    if(srvMoveJointx.call(srv))
    {         
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {     
        ROS_ERROR("Failed to call service dr_control_service : move_jointx\n");
        ros::shutdown();
        return -1;
    }
}
int amovejx(float fTargetPos[NUM_TASK], float fTargetVel, float fTargetAcc, float fTargetTime=0.f, float fBlendingRadius = 0.f,
           int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSolSpace = 0, int nSyncType = 1)
{
    return movejx(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, fBlendingRadius, nMoveReference, nMoveMode, nBlendingType, nSolSpace, /*nSyncType=*/ 1);
}


int movec(float fTargetPos[2][NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveCircle = node->serviceClient<dsr_msgs::MoveCircle>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_circle");

    dsr_msgs::MoveCircle srv; 
    std::vector<std_msgs::Float64MultiArray> poses;
    std_msgs::Float64MultiArray pos;
    for(int i=0; i<2; i++){
        pos.data.clear();
        for(int j = 0; j < NUM_TASK; j++){
            pos.data.push_back(fTargetPos[i][j]);
        }
        poses.push_back(pos);
    }

    ROS_INFO("  <xxx pos1> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",fTargetPos[0][0],fTargetPos[0][1],fTargetPos[0][2],fTargetPos[0][3],fTargetPos[0][4],fTargetPos[0][5]);
    ROS_INFO("  <xxx pos2> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",fTargetPos[1][0],fTargetPos[1][1],fTargetPos[1][2],fTargetPos[1][3],fTargetPos[1][4],fTargetPos[1][5]);

    ROS_INFO("  <pos1> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",poses[0].data[0],poses[0].data[1],poses[0].data[2],poses[0].data[3],poses[0].data[4],poses[0].data[5]);
    ROS_INFO("  <pos2> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",poses[1].data[0],poses[1].data[1],poses[1].data[2],poses[1].data[3],poses[1].data[4],poses[1].data[5]);

    srv.request.pos = poses;

    for(int i=0; i<2; i++){
        srv.request.vel[i] = fTargetVel[i];
        srv.request.acc[i] = fTargetAcc[i];
    }
    srv.request.time = fTargetTime;
    srv.request.radius = fBlendingRadius;
    srv.request.ref  = nMoveReference;
    srv.request.mode = nMoveMode;
    srv.request.blendType = nBlendingType;
    srv.request.syncType = nSyncType;

    ROS_INFO("service call: /dsr01m1013/motion/move_circle");
    ROS_INFO("  <pos1> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",srv.request.pos[0].data[0],srv.request.pos[0].data[1],srv.request.pos[0].data[2],srv.request.pos[0].data[3],srv.request.pos[0].data[4],srv.request.pos[0].data[5]);
    ROS_INFO("  <pos2> %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",srv.request.pos[1].data[0],srv.request.pos[1].data[1],srv.request.pos[1].data[2],srv.request.pos[1].data[3],srv.request.pos[1].data[4],srv.request.pos[1].data[5]);
    ROS_INFO("  <vel> %7.3f,%7.3f <acc> %7.3f,%7.3f <time> %7.3f",srv.request.vel[0],srv.request.vel[1],srv.request.acc[0],srv.request.acc[1], srv.request.time);
    ROS_INFO("  <mode> %d, <ref> %d, <radius> %7.3f, <blendType> %d",srv.request.mode,srv.request.ref, srv.request.radius, srv.request.blendType);

    if(srvMoveCircle.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_circle\n");
        ros::shutdown();
        return -1;
    }
}
int amovec(float fTargetPos[2][NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f, float fBlendingRadius = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nBlendingType = BLENDING_SPEED_TYPE_DUPLICATE, int nSyncType = 1)
{
    return movec(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, fBlendingRadius, nMoveReference, nMoveMode, nBlendingType, /*nSyncType=*/ 1);
}


int movesj(float fTargetPos[MAX_SPLINE_POINT][NUM_JOINT], int nPosCount, float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
           int nMoveMode = MOVE_MODE_ABSOLUTE, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveSplineJoint = node->serviceClient<dsr_msgs::MoveSplineJoint>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_spline_joint");
    dsr_msgs::MoveSplineJoint srv;
    std::vector<std_msgs::Float64MultiArray> poses;
    std_msgs::Float64MultiArray pos;

    for(int i = 0; i < MAX_SPLINE_POINT; i++){
        pos.data.clear();
        for(int j = 0; j < NUM_JOINT; j++){
            pos.data.push_back(fTargetPos[i][j]);
        }
        poses.push_back(pos);
    }
    srv.request.pos = poses;
    srv.request.posCnt = nPosCount;

    srv.request.vel = fTargetVel;
    srv.request.acc = fTargetAcc;
    srv.request.time = fTargetTime;
    srv.request.mode = nMoveMode;
    srv.request.syncType = nSyncType;

    if(srvMoveSplineJoint.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_spline_joint\n");
        ros::shutdown();
        return -1;
    }
}
int amovesj(float fTargetPos[MAX_SPLINE_POINT][NUM_JOINT], int nPosCount, float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
           int nMoveMode = MOVE_MODE_ABSOLUTE, int nSyncType = 1)
{
    return movesj(fTargetPos, nPosCount, fTargetVel, fTargetAcc, fTargetTime, nMoveMode, /*nSyncType=*/ 1);
}


int movesx(float fTargetPos[MAX_SPLINE_POINT][NUM_TASK], int nPosCount, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
           int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nVelOpt = SPLINE_VELOCITY_OPTION_DEFAULT, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveSplineTask = node->serviceClient<dsr_msgs::MoveSplineTask>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_spline_task");
    dsr_msgs::MoveSplineTask srv;
    std::vector<std_msgs::Float64MultiArray> poses;
    std_msgs::Float64MultiArray pos;

    for(int i = 0; i < MAX_SPLINE_POINT; i++){
        pos.data.clear();
        for(int j = 0; j < NUM_TASK; j++){
            pos.data.push_back(fTargetPos[i][j]);
        }
        poses.push_back(pos);
    }
    srv.request.pos = poses;
    srv.request.posCnt = nPosCount;
 
    for(int i=0; i<2; i++){
        srv.request.vel[i] = fTargetVel[i];
        srv.request.acc[i] = fTargetAcc[i];
    }
    srv.request.time = fTargetTime;
    srv.request.ref = nMoveReference;
    srv.request.mode = nMoveMode;
    srv.request.opt = nVelOpt;
    srv.request.syncType = nSyncType;

    if(srvMoveSplineTask.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_spline_task\n");
        ros::shutdown();
        return -1;
    }
}
int amovesx(float fTargetPos[MAX_SPLINE_POINT][NUM_TASK], int nPosCount, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
           int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nVelOpt = SPLINE_VELOCITY_OPTION_DEFAULT, int nSyncType = 1)
{
    return movesx(fTargetPos, nPosCount, fTargetVel,  fTargetAcc, fTargetTime, nMoveReference, nMoveMode, nVelOpt, /*nSyncType=*/ 1);
}


int moveb(MOVE_POSB* fTargetPos, int nPosCount, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveBlending = node->serviceClient<dsr_msgs::MoveBlending>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_blending");
    dsr_msgs::MoveBlending srv;

    std::vector<std_msgs::Float64MultiArray> segments;
    std_msgs::Float64MultiArray segment;

    for(int i=0; i<nPosCount; i++){
        segment.data.clear();

        for(int j=0; j<NUM_TASK; j++)
            segment.data.push_back( fTargetPos[i]._fTargetPos[0][j]);
        for(int j=0; j<NUM_TASK; j++)
            segment.data.push_back( fTargetPos[i]._fTargetPos[1][j]);

        segment.data.push_back( fTargetPos[i]._iBlendType );
        segment.data.push_back( fTargetPos[i]._fBlendRad  );  

        segments.push_back(segment);
    }
    srv.request.segment = segments;
    srv.request.posCnt = nPosCount;    

    for(int i=0; i<2; i++){
        srv.request.vel[i] = fTargetVel[i];
        srv.request.acc[i] = fTargetAcc[i];
    }
    srv.request.time = fTargetTime;    
    srv.request.ref = nMoveReference;
    srv.request.mode = nMoveMode;
    srv.request.syncType = nSyncType;

    if(srvMoveBlending.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_spline_blending\n");
        ros::shutdown();
        return -1;
    }

    return 0; 
}
int amoveb(MOVE_POSB* fTargetPos, int nPosCount, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
          int nMoveReference = MOVE_REFERENCE_BASE, int nMoveMode = MOVE_MODE_ABSOLUTE, int nSyncType = 1)
{
    return moveb(fTargetPos, nPosCount, fTargetVel, fTargetAcc, fTargetTime, nMoveReference, nMoveMode, /*nSyncType=*/ 1);
}


int move_spiral(float fRevolution, float fMaxRadius, float fMaxLength, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
               int nTaskAxis = TASK_AXIS_Z, int nMoveReference = MOVE_REFERENCE_TOOL, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveSpiral = node->serviceClient<dsr_msgs::MoveSpiral>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_spiral");
    dsr_msgs::MoveSpiral srv;

    srv.request.revolution = fRevolution;
    srv.request.maxRadius = fMaxRadius;
    srv.request.maxLength = fMaxLength;
    for(int i=0; i<2; i++){
        srv.request.vel[i] = fTargetVel[i];
        srv.request.acc[i] = fTargetAcc[i];
    }
    srv.request.time = fTargetTime;
    srv.request.taskAxis = nTaskAxis;
    srv.request.ref = nMoveReference;      
    srv.request.syncType = nSyncType;

    if(srvMoveSpiral.call(srv)){
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else{
        ROS_ERROR("Failed to call service dr_control_service : move_spiral\n");
        ros::shutdown();
        return -1;
    } 
}
int amove_spiral(float fRevolution, float fMaxRadius, float fMaxLength, float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
               int nTaskAxis = TASK_AXIS_Z, int nMoveReference = MOVE_REFERENCE_TOOL, int nSyncType = 1)
{
    return move_spiral(fRevolution, fMaxRadius, fMaxLength, fTargetVel, fTargetAcc, fTargetTime, nTaskAxis, nMoveReference, /*nSyncType=*/ 1);
}


int move_periodic(float fAmplitude[NUM_TASK], float fPeriodic[NUM_TASK], float fAccelTime = 0.f, int nRepeat = 1, int nMoveReference = MOVE_REFERENCE_TOOL, int nSyncType = 0)
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMovePeriodic = node->serviceClient<dsr_msgs::MovePeriodic>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_periodic");
    dsr_msgs::MovePeriodic srv;

    for(int i=0; i<NUM_TASK; i++){
        srv.request.amp[i] = fAmplitude[i];
        srv.request.periodic[i] = fPeriodic[i];
    }
    srv.request.acc = fAccelTime;
    srv.request.repeat = nRepeat;
    srv.request.ref = nMoveReference;
    srv.request.syncType = nSyncType;

    if(srvMovePeriodic.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_periodic\n");
        ros::shutdown();
        return -1;
    }

    return 0;
}
int amove_periodic(float fAmplitude[NUM_TASK], float fPeriodic[NUM_TASK], float fAccelTime = 0.f, int nRepeat = 1, int nMoveReference = MOVE_REFERENCE_TOOL, int nSyncType = 1)
{
    return move_periodic(fAmplitude, fPeriodic, fAccelTime, nRepeat, nMoveReference, /*nSyncType=*/ 1);
}


int move_wait()
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();  
    ros::ServiceClient srvMoveWait = node->serviceClient<dsr_msgs::MoveWait>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_wait");
    dsr_msgs::MoveWait srv;

    if(srvMoveWait.call(srv))
    {
        //ROS_INFO("receive srv, srv.response.success: %ld\n", (long int)srv.response.success);
        return (srv.response.success);
    }
    else
    {
        ROS_ERROR("Failed to call service dr_control_service : move_wait\n");
        ros::shutdown();
        return -1;
    }

    return 0; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void msgRobotState_cb(const dsr_msgs::RobotState::ConstPtr& msg)
{
    static int sn_cnt =0;
    
    sn_cnt++;
    if(0==(sn_cnt % 100))
    {  
        ROS_INFO("________ ROBOT STATUS ________");
        ROS_INFO("  robot_state       : %d", msg->robot_state);
        ROS_INFO("  robot_state_str   : %s", msg->robot_state_str.c_str());
        ROS_INFO("  current_posj      :  %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f",msg->current_posj[0] ,msg->current_posj[1] ,msg->current_posj[2]
                                                                             ,msg->current_posj[3] ,msg->current_posj[4] ,msg->current_posj[5] );
        ROS_INFO("  io_control_box    : %d", msg->io_control_box);
        //ROS_INFO("  io_modbus         : %d", msg->io_modbus);
        //ROS_INFO("  error             : %d", msg->error);
        ROS_INFO("  access_control    : %d", msg->access_control);
        ROS_INFO("  homming_completed : %d", msg->homming_completed);
        ROS_INFO("  tp_initialized    : %d", msg->tp_initialized);
        ROS_INFO("  speed             : %d", msg->speed);
        ROS_INFO("  mastering_need    : %d", msg->mastering_need);
        ROS_INFO("  drl_stopped       : %d", msg->drl_stopped);
        ROS_INFO("  disconnected      : %d", msg->disconnected);
    }
} 

void thread_subscriber()
{
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();
    ros::Subscriber subRobotState = node->subscribe("/"+ROBOT_ID +ROBOT_MODEL+"/state", 100, msgRobotState_cb);
 
    ros::MultiThreadedSpinner spinner(2);
    spinner.spin();
    
    /*
    ros::AsyncSpinner spinner(4); // Use 4 threads
    spinner.start();
    ros::waitForShutdown();
    */
}

void SigHandler(int sig)
{
    // Do some custom action.
    // For example, publish a stop message to some other nodes.
  
    // All the default sigint handler does is call shutdown()
    ROS_INFO("shutdown time! sig=%d",sig);
    ROS_INFO("shutdown time! sig=%d",sig);
    ROS_INFO("shutdown time! sig=%d",sig);
    //ros::ServiceClient srvMoveStop = nh.serviceClient<dsr_msgs::MoveStop>("/dsr01m1013/motion/move_stop");

    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();
    ros::Publisher pubRobotStop = node->advertise<dsr_msgs::RobotStop>("/"+ROBOT_ID +ROBOT_MODEL+"/stop",100);
    
    dsr_msgs::RobotStop msg;
    
    msg.stop_mode  = STOP_TYPE_QUICK;
    pubRobotStop.publish(msg);
    
    ros::shutdown();
}

int main(int argc, char** argv)
{
    //----- set target robot --------------- 
    string my_robot_id    = "dsr01";
    string my_robot_model = "m1013";
    if(1 == argc){
        ROS_INFO("default arguments: dsr01 m1013");
    }
    else{
        if(3 != argc){
            ROS_ERROR("invalid arguments: <ns> <model> (ex) dsr01 m1013");
            exit(1);
        }
        for (int i = 1; i < argc; i++){
            printf("argv[%d] = %s\n", i, argv[i]);
        }
        my_robot_id    = argv[1];
        my_robot_model = argv[2];
    }  
    //std::cout << "my_robot_id= " << my_robot_id << ", my_robot_model= " << my_robot_model << endl;
    SET_ROBOT(my_robot_id, my_robot_model);

    //----- init ROS ---------------------- 
    int rate_sub = 1;    // 1 Hz = 1 sec
    int nPubRate = 100;  // 100 Hz (10ms)
    int i=0, nRes=0; 
    ros::init(argc, argv, "dsr_basic_test_cpp", ros::init_options::NoSigintHandler);  
    ros::NodeHandle nh("~");
    // Override the default ros sigint handler.
    // This must be set after the first NodeHandle is created.
    signal(SIGINT, SigHandler);

    ros::Publisher  pubRobotStop = nh.advertise<dsr_msgs::RobotStop>("/"+ROBOT_ID +ROBOT_MODEL+"/stop",10);
    ///ros::Subscriber subRobotState = nh.subscribe("/"+ROBOT_ID +ROBOT_MODEL+"/state", 100, msgRobotState_cb);
    
    ros::ServiceClient srvMoveJoint = nh.serviceClient<dsr_msgs::MoveJoint>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_joint");
    ros::ServiceClient srvMoveLine = nh.serviceClient<dsr_msgs::MoveLine>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_line");
    ros::ServiceClient srvMoveCircle = nh.serviceClient<dsr_msgs::MoveCircle>("/"+ROBOT_ID +ROBOT_MODEL+"/motion/move_circle");

    // spawn another thread
    boost::thread thread_sub(thread_subscriber);

    /* 
    if(argc !=3)
    {
        ROS_INFO("cmd : rosrun dsr_control dsr_control_service arg0 arg1");
        ROS_INFO("arg0: double number, arg1: double number");
        return 1;
    }
    */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    float p1[6]={0.0,};                             //joint
    float p2[6]={0.0, 0.0, 90.0, 0.0, 90.0, 0.0};   //joint
    
    float x1[6]={400, 500, 800.0, 0.0, 180.0, 0.0}; //task
    float x2[6]={400, 500, 500.0, 0.0, 180.0, 0.0}; //task
    float velx[2]={50, 50};     // 태스크 속도를 50(mm/sec), 50(deg/sec)로 설정
    float accx[2]={100, 100};   // 태스크 가속도를 100(mm/sec2), 100(deg/sec2)로 설정

    float fCirclePos[2][6] = {{559,434.5,651.5,0,180,0}, {559,434.5,251.5,0,180,0}};
 
    while(ros::ok())
    {
      movej(p1, 30, 30); 
      movej(p2, 100, 100);
      
      movel(x1, velx, accx);
      movel(x2, velx, accx);

      movec(fCirclePos, velx, accx);
    }

    // wait the second thread to finish
    //thread_sub.join();
 
    ros::shutdown();
    //thread_sub.join();

    ROS_INFO("dsr_basic_test_cpp finished !!!!!!!!!!!!!!!!!!!!!");
    return 0;
}
