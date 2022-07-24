#include "RoboClaw.h"
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/UInt64.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#define Kp 1.0
#define Ki 0.5
#define Kd 0.25
#define qpps 44000
#define address 0x80
uint8_t lwheel;
uint8_t rwheel;
uint8_t lticks;
uint8_t rticks;
uint8_t lrate;
uint8_t rrate;
std_msgs::Int32 lwheel_ticks;
std_msgs::Int32 rwheel_ticks;
std_msgs::Float32 lwheel_rate;
std_msgs::Float32 rwheel_rate;

RoboClaw roboclaw(&Serial3,10000);

ros::NodeHandle nh;
//
//void lwheelCallback(  const std_msgs::Int32& lwheel_desired_rate)
//{
//  lwheel =  lwheel_desired_rate.data;
//}
//
//ros::Subscriber<geometry_msgs::Twist> sub1("lwheel_desired_rate" , lwheelCallback);
//
//void rwheelCallback(  const std_msgs::Int32& rwheel_desired_rate)
//{
//  rwheel =  rwheel_desired_rate.data;
//}
//
//ros::Subscriber<geometry_msgs::Twist> sub2("rwheel_desired_rate" , rwheelCallback);
void velCallback(  const geometry_msgs::Twist& cmd_vel)
{
lwheel = ((cmd_vel.linear.x - (cmd_vel.angular.z *0.21)) / 0.068) * 60/(2*3.14159);
rwheel = ((cmd_vel.linear.x + (cmd_vel.angular.z  *0.21)) / 0.068) * 60/(2*3.14159) ;
}
ros::Subscriber<geometry_msgs::Twist> cmd_vel("cmd_vel" , velCallback);
ros::Publisher pub1("lwheel_ticks", &lwheel_ticks);
ros::Publisher pub2("rwheel_ticks", &rwheel_ticks);
ros::Publisher pub3("lwheel_rate", &lwheel_rate);
ros::Publisher pub4("rwheel_rate", &rwheel_rate);

void setup()
{
  Serial.begin(9600);
  roboclaw.begin(38400);
  roboclaw.ResetEncoders(address);
 
  nh.initNode();
  nh.advertise(pub1);
  nh.advertise(pub2);
  nh.advertise(pub3);
  nh.advertise(pub4);
  nh.subscribe(cmd_vel);
//  nh.subscribe(sub1);
//  nh.subscribe(sub2);
}

void loop()
{
  roboclaw.ForwardM1(address,lwheel);
  
  roboclaw.ForwardM2(address,rwheel);
  
    
  lwheel_ticks.data = roboclaw.ReadEncM1(address,&lticks);
  rwheel_ticks.data = roboclaw.ReadEncM2(address,&rticks);
  lwheel_rate.data = roboclaw.ReadSpeedM1(address,&lrate);
  rwheel_rate.data = roboclaw.ReadSpeedM2(address,&rrate);

  Serial.println(lticks);
  
  pub1.publish(&lwheel_ticks);
  pub2.publish(&rwheel_ticks);
  pub3.publish(&lwheel_rate);
  pub4.publish(&rwheel_rate);
  nh.spinOnce();
  delay(1);
}
