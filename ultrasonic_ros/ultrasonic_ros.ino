/*
  :Version 1.0
  :Author: Dragos Calin
  :Email: dragos@intorobotics.com
  :License: BSD
  :Date: 01/05/2020
  :Last update: dd/mm/YYYY
*/
#include <NewPing.h>
#include <SimpleKalmanFilter.h>
#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

#define SONAR_NUM 4          //The number of sensors. 
#define MAX_DISTANCE 200     //Mad distance to detect obstacles.
#define PING_INTERVAL 33     //Looping the pings after 33 microseconds.

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

unsigned long _timerStart = 0;

int LOOPING = 40; //Loop for every 40 milliseconds.

uint8_t oldSensorReading[4];    //Store last valid value of the sensors.

uint8_t fl_sonar;             //Store raw sensor's value.
uint8_t fr_sonar;
uint8_t bl_sonar;
uint8_t br_sonar;

uint8_t fl_sonarKalman;       //Store filtered sensor's value.
uint8_t fr_sonarKalman;
uint8_t bl_sonarKalman;
uint8_t br_sonarKalman;


NewPing sonar[SONAR_NUM] = {
  NewPing(3, 2, MAX_DISTANCE), // Trigger pin, echo pin, and max distance to ping.
  NewPing(5, 4, MAX_DISTANCE),
  NewPing(7, 6, MAX_DISTANCE),
  NewPing(9, 8, MAX_DISTANCE)
};

/*
  create Kalman filter objects for the sensors.
   SimpleKalmanFilter(e_mea, e_est, q);
   e_mea: Measurement Uncertainty
   e_est: Estimation Uncertainty
   q: Process Noise
*/
SimpleKalmanFilter KF_fl(2, 2, 0.01);
SimpleKalmanFilter KF_fr(2, 2, 0.01);
SimpleKalmanFilter KF_bl(2, 2, 0.01);
SimpleKalmanFilter KF_br(2, 2, 0.01);

ros::NodeHandle nh; //create an object which represents the ROS node.

//looping the sensors
void sensorCycle() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle();
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = 0;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
}

// If ping received, set the sensor distance to array.
void echoCheck() {
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

//Return the last valid value from the sensor.
void oneSensorCycle() {
  fl_sonar   = returnLastValidRead(0, cm[0]);
  fr_sonar = returnLastValidRead(1, cm[1]);
  bl_sonar  = returnLastValidRead(2, cm[2]);
  br_sonar  = returnLastValidRead(2, cm[3]);
}

//If sensor value is 0, then return the last stored value different than 0.
int returnLastValidRead(uint8_t sensorArray, uint8_t cm) {
  if (cm != 0) {
    return oldSensorReading[sensorArray] = cm;
  } else {
    return oldSensorReading[sensorArray];
  }
}

//Apply Kalman Filter to sensor reading.
void applyKF() {
  fl_sonarKalman   = KF_fl.updateEstimate(fl_sonar);
  fr_sonarKalman = KF_fr.updateEstimate(fr_sonar);
  bl_sonarKalman  = KF_bl.updateEstimate(bl_sonar);
  br_sonarKalman  = KF_br.updateEstimate(br_sonar);
}

void startTimer() {
  _timerStart = millis();
}

bool isTimeForLoop(int _mSec) {
  return (millis() - _timerStart) > _mSec;
}

void sensor_msg_init(sensor_msgs::Range &range_name, char *frame_id_name)
{
  range_name.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_name.header.frame_id = frame_id_name;
  range_name.field_of_view = 0.26;
  range_name.min_range = 0.0;
  range_name.max_range = 2.0;
}

//Create three instances for range messages.
sensor_msgs::Range range_fl;
sensor_msgs::Range range_fr;
sensor_msgs::Range range_bl;
sensor_msgs::Range range_br;

//Create publisher onjects for all sensors
ros::Publisher pub_range_fl("/ultrasound_fl", &range_fl);
ros::Publisher pub_range_fr("/ultrasound_fr", &range_fr);
ros::Publisher pub_range_bl("/ultrasound_bl", &range_bl);
ros::Publisher pub_range_br("/ultrasound_br", &range_br);

void setup() {
  pingTimer[0] = millis() + 75;
  for (uint8_t i = 1; i < SONAR_NUM; i++)
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;

  nh.initNode();
  nh.advertise(pub_range_fl);
  nh.advertise(pub_range_fr);
  nh.advertise(pub_range_bl);
  nh.advertise(pub_range_br);

  sensor_msg_init(range_fl, "/ultrasound_fl");
  sensor_msg_init(range_fr, "/ultrasound_fr");
  sensor_msg_init(range_bl, "/ultrasound_bl");
  sensor_msg_init(range_br, "/ultrasound_br");
}

void loop() {
  if (isTimeForLoop(LOOPING)) {
    sensorCycle();
    oneSensorCycle();
    applyKF();
    range_fl.range   = fl_sonarKalman;
    range_fr.range = fr_sonarKalman;
    range_bl.range  = bl_sonarKalman;
    range_br.range  = br_sonarKalman;

    range_fl.header.stamp = nh.now();
    range_fr.header.stamp = nh.now();
    range_bl.header.stamp = nh.now();
    range_br.header.stamp = nh.now();

    pub_range_fl.publish(&range_fl);
    pub_range_fr.publish(&range_fr);
    pub_range_bl.publish(&range_bl);
    pub_range_br.publish(&range_br);

    startTimer();
  }
  nh.spinOnce();//Handle ROS events
}
