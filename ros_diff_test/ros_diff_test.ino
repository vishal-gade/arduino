#include "RoboClaw.h" 
#include <ros.h>
#include <ros/time.h>
#include <ros_arduino_base/UpdateGains.h>
#include <ros_arduino_msgs/Encoders.h>
#include <ros_arduino_msgs/CmdDiffVel.h>
#define BAUD 115200
#define address 0x80

RoboClaw roboclaw(&Serial3,10000);


char frame_id[] = "base_footprint";

// Vehicle characteristics
float counts_per_rev[1];
float wheel_radius[1];         // [m]
float meters_per_counts;       // [m/counts]
int pwm_range[1];

float speedl=0,speedr=0;
int control_rate[1];   // [Hz]
int encoder_rate[1];   // [Hz]
int no_cmd_timeout[1]; // [seconds]

uint32_t up_time;             // [milliseconds]
uint32_t last_encoders_time;  // [milliseconds]
uint32_t last_cmd_time;       // [milliseconds]
uint32_t last_control_time;   // [milliseconds]
uint32_t last_status_time;    // [milliseconds]

// ROS node
ros::NodeHandle_<ArduinoHardware, 10, 10, 1024, 1024> nh;
/*// ROS services prototype
void updateGainsCb(const ros_arduino_base::UpdateGains::Request &req, ros_arduino_base::UpdateGains::Response &res);
// ROS services
ros::ServiceServer<ros_arduino_base::UpdateGains::Request, ros_arduino_base::UpdateGains::Response> update_gains_server("update_gains", &updateGainsCb);
*/
// ROS subribers/service callbacks prototye
void cmdDiffVelCallback(const ros_arduino_msgs::CmdDiffVel& diff_vel_msg); 

// ROS subsribers
ros::Subscriber<ros_arduino_msgs::CmdDiffVel> sub_diff_vel("cmd_diff_vel", cmdDiffVelCallback);
// ROS publishers msgs
ros_arduino_msgs::Encoders encoders_msg;
// ROS publishers
ros::Publisher pub_encoders("encoders", &encoders_msg);

float pid_gains_left[3];
float pid_gains_right[3];
float Kp_l, Ki_l, Kd_l;
float Kp_r, Ki_r, Kd_r;
void setup()
{
  Serial.begin(9600); 
  nh.getHardware()->setBaud(BAUD);
  nh.initNode();
  encoders_msg.header.frame_id = frame_id;
  nh.advertise(pub_encoders);
  nh.subscribe(sub_diff_vel);
  //nh.advertiseService(update_gains_server);
  void updateGainsCb(const ros_arduino_base::UpdateGains::Request &req, ros_arduino_base::UpdateGains::Response &res);
// ROS services
ros::ServiceServer<ros_arduino_base::UpdateGains::Request, ros_arduino_base::UpdateGains::Response> update_gains_server("update_gains", &updateGainsCb);
  
  // Wait for ROSserial to connect
  while (!nh.connected()) 
  {
    nh.spinOnce();
  }
  nh.loginfo("Connected to microcontroller.");
  Serial3.begin(19200);  
  // Load parameters 
  loadParams();
  roboclaw.begin(38400);

}

void loop()
{
 if ((millis() - last_encoders_time) >= (1000 / encoder_rate[0]))
  { 
    encoders_msg.left = ReadEncM1(address);
    encoders_msg.right = ReadEncM2(address);
    encoders_msg.header.stamp = nh.now();
    pub_encoders.publish(&encoders_msg);
    last_encoders_time = millis();
  }

  nh.spinOnce();
}

 
void cmdDiffVelCallback( const ros_arduino_msgs::CmdDiffVel& diff_vel_msg) 
{
  speedr = 1500*diff_vel_msg.left;
  speedl = 1500*diff_vel_msg.right;
  last_cmd_time = millis();
}

void loadParams() {
  /*nh.getParam("control_rate", control_rate,1);
  nh.getParam("encoder_rate", encoder_rate,1);
  nh.getParam("no_cmd_timeout", no_cmd_timeout,1);;
  nh.getParam("counts_per_rev", counts_per_rev,1);
  nh.getParam("wheel_radius", wheel_radius,1);
  nh.getParam("pwm_range", pwm_range,1);*/
  wheel_radius[0] =  0.0635 ;
  counts_per_rev[0]= 2400;
  
  control_rate[0]=50;
  encoder_rate[0] =50;
  pwm_range[0] =1023;
  no_cmd_timeout[0] =1;
  
  // Compute the meters per count
  meters_per_counts = ((PI * 2 * wheel_radius[0]) / counts_per_rev[0]);
}
void updateGainsCb(const ros_arduino_base::UpdateGains::Request & req, ros_arduino_base::UpdateGains::Response & res)
{
  for ( int x = 0; x < 3; x++)
  {
    pid_gains_left[x] = req.gains[x];
    pid_gains_right[x] = req.gains[x+3];
  }
  
  Kp_l = pid_gains_left[0];
  Ki_l = pid_gains_left[1] / control_rate[0];
  Kd_l = pid_gains_left[2] * control_rate[0];
  
  Kp_r = pid_gains_right[0];
  Ki_r = pid_gains_right[1] / control_rate[0];
  Kd_r = pid_gains_right[2] * control_rate[0];
}
