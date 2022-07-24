#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int c = 0;//counter
int random_end;

void setup() {
  Serial.begin(9600);
  
  pwm.begin();
  pwm.setPWMFreq(480);  // 60Hz to 600Hz range
//  Wire.setClock(400000);
}

void loop() {

  random_end = random(4096);
  //Serial.println(random_end);
  
  //loop once and never again - init 
  if(c==0){
    for (uint16_t i=4096; i>0; i -= 8) {
      for (uint8_t pwmnum=0; pwmnum < 6; pwmnum++) {
        pwm.setPWM(pwmnum, 0,(i + (4096/16)*pwmnum) % 4096);
      }
    }
    c++;
  }
  //random loop start 
  for (uint16_t i=0; i<random_end; i += 8) {
    for (uint8_t pwmnum=0; pwmnum < 6; pwmnum++) {
      pwm.setPWM(pwmnum, 0, (i + (4096/16)*pwmnum) % 4096 );
      }
  }
  delay(500);
  for (uint16_t i=0; i>0; i -= 8) {
    for (uint8_t pwmnum=0; pwmnum < 6; pwmnum++) {
      pwm.setPWM(pwmnum, 0, (i + (4096/16)*pwmnum) % 4096 );
      }
  }

  


  
}//void end





//(i + (4096/16)*pwmnum) % 4096
//
//void len_to_pwm(int actuator_len){
//  pwm = map(actuator_len,0,50,0,4096);
//  }
