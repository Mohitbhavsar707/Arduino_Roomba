#include <IRremote.h>
#include <Servo.h>


//Switch
int switchPin = 7;
bool on = false;
int switchState = 0;

  
//Lights
int redPin = 2;
int greenPin = 8;
int yellowPin = 4;
int blueLightOne = A5;
int blueLightTwo = 13;


//DC Motors
int leftOne = 3; // forward
int leftTwo = 5;

int rightOne = 6; // forward
int rightTwo = 9;

int vaccumPin = 10;
int normalSpeed = 255;


//Distance sensor
int distanceOneCm = 0;
int distanceTwoCm = 0;
int centerDistanceCm = 0;


//Photoresistor sensor
int sensorPin = A0;  
int lightSensorValue = 0;

//Flex Sensor
int flexSensorValue = 0;
int flexPin = A1;


//servo
Servo servo_A4;
int pos = 0;


//IR Remote
int RECV_PIN = A2;
IRrecv irrecv(RECV_PIN);
decode_results results;

int randomTurn;


void setup()
{
  
  //Setting up all the pin Inputs,Outputs
  
  
  //Lights
  pinMode(switchPin, INPUT);
  pinMode(redPin, OUTPUT); 	
  pinMode(greenPin, OUTPUT); 	
  pinMode(yellowPin, OUTPUT); 	
  pinMode(blueLightOne, OUTPUT);
  pinMode(blueLightTwo, OUTPUT);
  
  //Motors
  pinMode(leftOne, OUTPUT); 	
  pinMode(leftTwo, OUTPUT); 	
  pinMode(rightOne, OUTPUT); 	
  pinMode(rightTwo, OUTPUT); 	
  pinMode(vaccumPin, OUTPUT); 	
  
  //Servo
  servo_A4.attach(A4, 500, 2500);
  servo_A4.write(90);

  
  irrecv.enableIRIn(); // Start the receiver

  
  Serial.begin(9600);

  
}

void loop()
{
  switchState = digitalRead(7); // switch behaviour
  randomTurn = random(0,100); // picks a random turn to be made

  //Distance Sensing
  distanceOneCm = 0.01723 * readUltrasonicDistance(12, 12);
  distanceTwoCm = 0.01723 * readUltrasonicDistance(11, 11);
  centerDistanceCm = 0.01723 * readUltrasonicDistance(A3, A3);
  
  //Light sensing under object (E.g Stairs)
  lightSensorValue = analogRead(sensorPin);	
  lightSensorValue = map(lightSensorValue, 6, 679, 0, 100); // map changes the values of the lightSensor to a more usable number (0 - 100)
  
  //Carpet Sensing
  flexSensorValue = analogRead(flexPin); // sets the sensor to read pin 11
  flexSensorValue = map(flexSensorValue, 6, 33, 0 ,30); // map changes the flex sensor values to usable numbers

  
 //IR remote
  if (irrecv.decode(&results)) {
  	  irrecv.resume(); // Waiting to receive the next value 
  }
  
    if (switchState == HIGH){ // switch on/off
     	
      onStart(); // method call 
      
      
      //if the distance is less than x, then the bot reverses and turns accordingly
      if(centerDistanceCm < 30 || lightSensorValue > 80 || flexSensorValue < 15 ){ 
        	driveReverse();
        	objectDectectionLightYellow();
        
        	if ((int)randomTurn % 2 == 0){
             	turnLeftServo();

    		} else {
              	turnRightServo();
    		}
        
      } else {
 			driveForward();
            servo_A4.write(90);
      }
      
      if(centerDistanceCm < 30){
        
      }
      
      
      //checks for left distance sensor
       if(distanceOneCm < 30){ 
        	driveReverse();
        	objectDectectionLightBlueOne();
         	turnRightServo();
       } 
      
      //checks for right distance sensor
      if(distanceTwoCm < 30){ 
        	driveReverse();
        	objectDectectionLightBlueTwo();
         	turnLeftServo();
       } 
      
      
      //checks for remote values
      if(results.value == 0xFD807F  ){
        driveForward();
      
      } else if(results.value == 0xFD906F   ){
        driveReverse();
  	    objectDectectionLightYellow();

      
      } else if(results.value == 0xFD609F  ){
 		turnRightServo();
        objectDectectionLightBlueOne();


      } else if(results.value == 0xFD20DF   ){
   		turnLeftServo();
        objectDectectionLightBlueTwo();

      }

     

    } else {
      whenClosing(); // method call to when rooomba is turned off
    }
}


long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}


void onStart(){
	  digitalWrite(greenPin, HIGH); // turn green light on
      digitalWrite(redPin, LOW); // turn red light off
      analogWrite(vaccumPin, 255); // vaccum on  
}


void driveForward(){
  Serial.println("Driving Forward");
  analogWrite(leftOne, normalSpeed); // turn left motor on going forward
  analogWrite(rightOne, normalSpeed); // turn right motor on going forward
  analogWrite(leftTwo, 0); // set reverse for left motor to 0
  analogWrite(rightTwo, 0); // set reverse for right motor to 0
}


void whenClosing(){
  Serial.println("closed");
  digitalWrite(greenPin, LOW); // turn green light off
  digitalWrite(redPin, HIGH); // turn red light on
  analogWrite(vaccumPin, 0); // vaccum off
  
  //turns all motors off
  analogWrite(leftTwo, 0);  
  analogWrite(rightTwo, 0);	 
  analogWrite(leftOne, 0);  
  analogWrite(rightOne, 0);
	  
}

void driveReverse(){
  	Serial.println("Reversing");
  	analogWrite(leftTwo, 255); // set left motor reverse to occur
 	analogWrite(rightTwo, 255);	// set right motor reverse to occur
  	analogWrite(leftOne, 0); // set left forward to 0
  	analogWrite(rightOne, 0); // set right forward to 0 
    
}


void objectDectectionLightYellow(){ // bliking yellow light
  digitalWrite(4, HIGH);
  //delay(10);
  digitalWrite(4, LOW);
}

void objectDectectionLightBlueOne(){ // blinking blue light
  digitalWrite(A5, HIGH);
  //delay(10);
  digitalWrite(A5, LOW);
}

void objectDectectionLightBlueTwo(){ // blinking blue light
  digitalWrite(A4, HIGH);
  //delay(10);
  digitalWrite(A4, LOW);
}

void turnLeftServo(){
  Serial.println("Turning Left");
   	 for (pos = 90; pos >= 0; pos -= 1) {
     // tell servo to go to position in variable 'pos'
     servo_A4.write(pos);
     // wait 15 ms for servo to reach the position
     delay(15); // Wait for 15 millisecond(s)
   }
}

void turnRightServo(){
   Serial.println("Turning Right");

   for (pos = 180; pos >= 90; pos -= 1) {
     // tell servo to go to position in variable 'pos'
     servo_A4.write(pos);
     // wait 15 ms for servo to reach the position
     delay(15); 
   }
}


