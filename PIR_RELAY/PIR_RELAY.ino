/*
 * PIR sensor tester
 */
 
int ledPin = 12;  
int buzzer =4;                  // choose the pin for the LED
int inputPin2 = 2;               // choose the input pin (for PIR sensor)
int inputPin3 = 3;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val2 = 0;                    // variable for reading the pin status
int val3 = 0;                    // variable for reading the pin status
 
void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin2, INPUT);     // declare sensor as input
  pinMode(inputPin3, INPUT);     // declare sensor as input  
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(buzzer, LOW);
  Serial.begin(9600);
}
 
void loop(){
val2 = digitalRead(inputPin2);  // read input value
  val3 = digitalRead(inputPin3);  // read input value
  if (val2 == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, LOW);  // turn LED ON
     digitalWrite(buzzer, HIGH);
    if (pirState == LOW) {
      // we have just turned on
//      Serial.println("pin 2 Motion detected!");
      // We only want to print on the output change, not state

      pirState = HIGH;
    }
  } 
  else if (val3 == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, LOW);  // turn LED ON
    digitalWrite(buzzer, HIGH);
    if (pirState == LOW) {
      // we have just turned on
//      Serial.println("pin 3 Motion detected!");
      // We only want to print on the output change, not state
    
      pirState = HIGH;
    }
  }  
  else {
    digitalWrite(ledPin, HIGH); // turn LED OFF
    digitalWrite(buzzer, LOW);
    if (pirState == HIGH){
      // we have just turned of
//      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      
      pirState = LOW;
    }
  }
}
