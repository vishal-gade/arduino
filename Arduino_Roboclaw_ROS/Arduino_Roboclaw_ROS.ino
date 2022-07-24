#include "RoboClaw.h"
#include <ros.h>
#include <std_msgs/UInt64.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#include <SoftwareSerial.h>
RoboClaw roboclaw(&Serial3,10000);
//SoftwareSerial serial(10,11);  
//RoboClaw roboclaw(&serial,10000);
#define address 0x80
double Speed = 64;
double speed_req;                        
double angular_speed_req;
double speed_req_left; 
double speed_req_right;  
const double wheelbase = 0.54;
ros::NodeHandle nh;

void velCallback(  const geometry_msgs::Twist& cmd_vel)
{
  
  speed_req = cmd_vel.linear.x;                                     //Extract the commanded linear speed from the message

  angular_speed_req = cmd_vel.angular.z;                            //Extract the commanded angular speed from the message
  
  speed_req_left = speed_req - angular_speed_req*(wheelbase/2);     //Calculate the required speed for the left motor to comply with commanded linear and angular speeds
  speed_req_right = speed_req + angular_speed_req*(wheelbase/2);    //Calculate the required speed for the right motor to comply with commanded linear and angular speeds

}

ros::Subscriber<geometry_msgs::Twist> cmd_vel("cmd_vel" , velCallback);
void setup() {
  Serial.begin(9600);
  roboclaw.begin(38400);

  //stop
  roboclaw.ForwardMixed(address, 0);
  roboclaw.TurnRightMixed(address, 0);
  
  nh.initNode();
  nh.subscribe(cmd_vel);
}

void loop() {
  if(speed_req ==0){
    if (angular_speed_req ==0){
      
    roboclaw.ForwardM1(address,0); 
    roboclaw.ForwardM2(address,0);
      }
    else if(angular_speed_req == -1){
       roboclaw.ForwardM1(address,Speed); 
    roboclaw.BackwardM2(address,Speed);
      }
    else{
       roboclaw.BackwardM1(address,Speed); 
    roboclaw.ForwardM2(address,Speed);}
    }
  else{
     if (angular_speed_req ==0){
      
    roboclaw.ForwardM1(address,100); 
    roboclaw.ForwardM2(address,100);
      }
    else if(angular_speed_req == -1){
       roboclaw.ForwardM1(address,Speed); 
    roboclaw.ForwardM2(address,Speed/2.0);
      }
    else{
       roboclaw.ForwardM1(address,Speed/0.2); 
    roboclaw.ForwardM2(address,Speed);}
    }



  nh.spinOnce();
  delay(1);

}
