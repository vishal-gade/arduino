/*
  This code should work to get warning cross the buzzer when something be closer than 0.5 meter
  Circuit is ultrasonic sensor and buzzer +5v and Arduino uno is used
  a_atef45@yahoo.com
  www.zerosnones.net
  +201153300223
*/
// Define pins for ultrasonic and buzzer
int const trigPin = 10;
int const echoPin = 9;
const int buzzer = 5; //buzzer to arduino pin 9

void setup()
{
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // trig pin will have pulses output
  pinMode(echoPin, INPUT); // echo pin should be input to get pulse width
  pinMode(buzzer, OUTPUT); // buzz pin is output to control buzzering
}

void loop()
{
  // Duration will be the input pulse width and distance will be the distance to the obstacle in centimeters
  int duration, distance;
  // Output pulse with 1ms width on trigPin
  digitalWrite(trigPin, HIGH); 
  delay(1);
  digitalWrite(trigPin, LOW);
  // Measure the pulse input in echo pin
  duration = pulseIn(echoPin, HIGH);
  // Distance is half the duration devided by 29.1 (from datasheet)
  distance = (duration/2) / 29.1;
  Serial.println(distance);
  // if distance less than 0.5 meter and more than 0 (0 or less means over range) 
    if (distance <= 50 && distance >= 0) {
      // Buzz
      digitalWrite(buzzer, HIGH);
    } else {
      // Don't buzz
      digitalWrite(buzzer, LOW);
    }
    // Waiting 60 ms won't hurt any one
    delay(60);
}
