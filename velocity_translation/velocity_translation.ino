#include "RoboClaw.h"
#include <ros.h>
#include <std_msgs/UInt64.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>

RoboClaw roboclaw(&Serial3,10000);

#define address 0x80
double Speed = 64;
double linear_speed;                        
double angular_speed;
double speed_req_left; 
double speed_req_right;  
const double wheelbase = 0.04;
ros::NodeHandle nh;

void velCallback(  const geometry_msgs::Twist& cmd_vel)
{
  linear_speed = cmd_vel.linear.x;                                   

  angular_speed = cmd_vel.angular.z;                           
  
  speed_req_left = 32*(linear_speed - (angular_speed * 0.21));    
  speed_req_right = 32*(linear_speed + (angular_speed * 0.21)); 
  
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
 uint8_t status1,status2,status3,status4;
  bool valid1,valid2,valid3,valid4;
  
  int32_t enc1= roboclaw.ReadEncM1(address, &status1, &valid1);
  int32_t enc2 = roboclaw.ReadEncM2(address, &status2, &valid2);
  int32_t speed1 = roboclaw.ReadSpeedM1(address, &status3, &valid3);
  int32_t speed2 = roboclaw.ReadSpeedM2(address, &status4, &valid4);

  
 roboclaw.ForwardM1(address,speed_req_left);
 roboclaw.ForwardM2(address,speed_req_right);
  nh.spinOnce();
  delay(1);

}
